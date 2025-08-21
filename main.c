#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>   // calloc, free
#include <string.h>   // strcmp
#include <getopt.h>   // getopt_long, struct option

#include "lbrctl_common.h"

/* forward decls from the other units */
int cmd_config(const char* dev, int argc, char** argv);
int cmd_run   (const char* dev, int argc, char** argv);

/*
 * Helper: wrap a subcommand's argv so that argv[0] is a program name.
 * This makes GNU getopt_long behave consistently inside the subcommand.
 */
static int dispatch_subcommand(
    const char* progname,
    int (*fn)(const char* dev, int argc, char** argv),
    const char* dev,
    int argc, char** argv)
{
    int argc_sub = argc + 1;              /* +1 for argv[0] placeholder */
    char** av = (char**)calloc((size_t)argc_sub, sizeof(char*));
    if (!av) { perror("calloc"); return 1; }

    av[0] = (char*)progname;              /* synthetic argv[0] */
    for (int i = 1; i < argc_sub; ++i)
        av[i] = argv[i - 1];

    /* Ensure clean getopt state for the subcommand */
    optind = 0;

    int rc = fn(dev, argc_sub, av);
    free(av);
    return rc;
}

int main(int argc, char** argv) {
    const char* dev = NULL; /* device path if provided */

    /* global long options: only --dev here */
    static struct option global[] = {
        {"dev", required_argument, 0, 1000},
        {0,0,0,0}
    };

    int idx = 0, c;
    while ((c = getopt_long(argc, argv, "", global, &idx)) != -1) {
        if (c == 1000) dev = optarg;
        else { usage(stderr); return 2; }
    }

    if (optind >= argc) {
        usage(stderr);
        return 2;
    }

    const char* cmd = argv[optind++];

    /* Remaining args after the subcommand token */
    int   argc_rest = argc - optind;
    char** argv_rest = argv + optind;

    if (!strcmp(cmd, "config"))
        return dispatch_subcommand("lbrctl-config", cmd_config, dev, argc_rest, argv_rest);

/* IMPORTANT: for 'run' pass argv_rest as-is so argv_rest[0] is exactly "--" */
    if (!strcmp(cmd, "run"))
        return cmd_run(dev, argc_rest, argv_rest);

    usage(stderr);
    return 2;

}

