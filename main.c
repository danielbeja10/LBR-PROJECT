#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>   // calloc, free
#include <string.h>   // strcmp
#include <getopt.h>   // getopt_long, struct option

#include "lbrctl_common.h"

int cmd_config(const char* dev, int argc, char** argv);
int cmd_run   (const char* dev, int argc, char** argv);


int main(int argc, char** argv) {
    const char* dev = NULL;  
    
    // The first argument must be the subcommand: "config" or "run".
    if (argc < 2) {
        usage(stderr);
        return 2;
    }

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


    fprintf(stderr, "Unknown subcommand: %s\n", cmd);
    usage(stderr);
    return 2;

}
