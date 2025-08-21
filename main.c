#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>   // calloc, free
#include <string.h>   // strcmp
#include <getopt.h>   // getopt_long, struct option

#include "lbrctl_common.h"

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
    const char* dev = NULL;  

    // The first argument must be the subcommand: "config" or "run".
    if (argc < 2) {
        usage(stderr);
        return 2;
    }

<<<<<<< HEAD
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
=======
    const char* cmd = argv[1];

    // Handle "config" subcommand
    if (strcmp(cmd, "config") == 0) {
        // Pass the rest of the arguments (after "config") to cmd_config
        return cmd_config(dev, argc - 2, argv + 2);
    }

    // Handle "run" subcommand
    if (strcmp(cmd, "run") == 0) {
        // Pass the rest of the arguments (after "run") to cmd_run
        return cmd_run(dev, argc - 2, argv + 2);
    }
>>>>>>> 9b68b98 (after changes 9.9)


    fprintf(stderr, "Unknown subcommand: %s\n", cmd);
    usage(stderr);
    return 2;

}

