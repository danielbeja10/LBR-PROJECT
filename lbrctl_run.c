#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <getopt.h>

#include "lbr_API.h"
#include "lbrctl_common.h"

/* 
* Run the desired application.
* Capture its braches. (Enable LBR)
* Print branches into json file.
* Stop the recording (Disable LBR)
*/
int cmd_run(const char* dev, int argc, char** argv) {

    (void)argc; (void)argv;
    int i = 0;
    /*
    * Recognize the "--" in order to find the program that we want to capture.
    */
    while (argv[i] && strcmp(argv[i], "--") != 0) i++;
    if (!argv[i] || !argv[i+1]) {
        fprintf(stderr, "run: missing \"-- <program>\"\n");
        return 2;
    }
    char** prog_argv = &argv[i+1]; // set as the path to the program.

    int fd = open_dev(dev);
    if (fd < 0) return 1;


    /*
    * Check LBR availability.
    */
    struct lbr_basic_report rep = (struct lbr_basic_report){0};
    if (ioctl(fd, LBR_IOCTL_GET_BASIC, &rep) != 0) {
        fprintf(stderr, "GET_BASIC failed: %s\n", strerror(errno));
        close(fd);
        return 1;
    }
    if (!rep.has_lbr) 
    { 
        fprintf(stderr, "LBR not available on this system.\n"); 
        close(fd); 
        return 1; 
    }
    /*
    * Enable LBR.
    */
    if (ioctl(fd, LBR_IOCTL_ENABLE) != 0) {
        fprintf(stderr, "ENABLE failed: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

   
    /*
    * Use fork to run the desired program.
    * The programs run, while LBR capture its branches.
    * if it succeed parent terminate and we documnet the info for the user.
    */
    pid_t child = fork();                  // split into parent/child
    if (child < 0) 
    { 
        perror("fork"); ioctl(fd, LBR_IOCTL_DISABLE); 
        close(fd); 
        return 1; 
    }
    if (child == 0) {                      // child runs the target
        execvp(prog_argv[0], prog_argv);
        perror("execvp");
        _exit(127);
    }

    int status = 0;
    (void)waitpid(child, &status, 0);      // parent waits
    (void)ioctl(fd, LBR_IOCTL_DISABLE);    // stop capture


    /*
    * Copying what we captured for the user.
    * building the dump.
    */
    unsigned cap = rep.lbr_limits.max_depth_supported;
    if (cap == 0)
        cap = rep.lbr_limits.current_depth;
    if (cap == 0)
    cap = 32;    

    

    struct lbr_entry_uapi* buf = calloc(cap, sizeof(*buf)); // allocate aaray for capture buf
    if (!buf) 
    { 
        fprintf(stderr, "OOM\n"); 
        ioctl(fd, LBR_IOCTL_DISABLE); 
        close(fd); 
        return 1; 
    }

    struct lbr_dump_req req = (struct lbr_dump_req){0};
    req.buf   = (unsigned long long)(uintptr_t)buf; // pointer to FROM TO arrays.
    req.max   = cap; // Number of the branches.
    req.clear = 1; // clear the LBR after copying.

    if (ioctl(fd, LBR_IOCTL_DUMP_ENTRIES, &req) != 0) {
        fprintf(stderr, "DUMP_ENTRIES failed: %s\n", strerror(errno));
        free(buf);
        ioctl(fd, LBR_IOCTL_DISABLE);
        close(fd);
        return 1;
    }

    
    close(fd);

    write_json(stdout, buf, req.count);    // JSON out
    free(buf);

    if (WIFEXITED(status))  
         return WEXITSTATUS(status);
    if (WIFSIGNALED(status)) 
        return 128 + WTERMSIG(status);
    return 0;
}
