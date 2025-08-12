#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "lbrctl_common.h"

/* forward decls from the other units */
int cmd_config(const char* dev, int argc, char** argv);
int cmd_run   (const char* dev, int argc, char** argv);

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

    if (optind >= argc) 
    { 
        usage(stderr); 
        return 2; 
    }

    const char* cmd = argv[optind++];
    if (!strcmp(cmd, "config")) return cmd_config(dev, argc - optind, argv + optind);
    if (!strcmp(cmd,  "run"))   return cmd_run   (dev, argc - optind, argv + optind);

    usage(stderr);
    return 2;
}