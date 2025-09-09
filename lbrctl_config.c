#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "lbrctl_common.h"
#include "lbr_API.h"  

/* 
* Create the configuration of the user.
* Without enabling LBR and without running the desired application.
*/
int cmd_config(const char* dev, int argc, char** argv) {
    int set_depth = 0;          
    int usr = 0, ker = 0, callstack = 0;
    int filter = 0;             
    int fbits[7] = {0};         

    for (int i = 0; i < argc; i++){ 
        
        const char* tok = argv[i];

        if (strcmp(tok, "depth") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "config: missing value after 'depth'\n");
                return 2;
            }
            char* endp = NULL;
            unsigned long v = strtoul(argv[++i], &endp, 0);
            if (endp == argv[i] || *endp != '\0' ) {
                fprintf(stderr, "config: invalid depth '%s'\n", argv[i]);
                return 2;
            }
            set_depth = (int)v;
        }
        else if (strcmp(tok, "usr") == 0) {
            usr = 1;
        }
        else if (strcmp(tok, "ker") == 0) {
            ker = 1;
        }
        else if (strcmp(tok, "callstack") == 0) {
            callstack = 1;
        }
        else if (strcmp(tok, "filter1") == 0) { filter = 1; fbits[0] = 1; }
        else if (strcmp(tok, "filter2") == 0) { filter = 1; fbits[1] = 1; }
        else if (strcmp(tok, "filter3") == 0) { filter = 1; fbits[2] = 1; }
        else if (strcmp(tok, "filter4") == 0) { filter = 1; fbits[3] = 1; }
        else if (strcmp(tok, "filter5") == 0) { filter = 1; fbits[4] = 1; }
        else if (strcmp(tok, "filter6") == 0) { filter = 1; fbits[5] = 1; }
        else if (strcmp(tok, "filter7") == 0) { filter = 1; fbits[6] = 1; }
        else {
            fprintf(stderr, "config: unknown token '%s'\n", tok);
            return 2;
        }
    }

    int fd = open_dev(dev);
    if (fd < 0) return 1;

    struct lbr_basic_report rep = {0};
    if (ioctl(fd, LBR_IOCTL_GET_BASIC, &rep) != 0) {
        fprintf(stderr, "GET_BASIC failed: %s\n", strerror(errno));
        close(fd);
        return 1;
    }
    if (!rep.has_lbr) {
        fprintf(stderr, "LBR not available on this system.\n");
        close(fd);
        return 1;
    }
    /*
    * Set the depth of the LBR.
    */
    if (set_depth > 0) { 
        __u32 d = set_depth;
        if (ioctl(fd, LBR_IOCTL_SET_DEPTH, &d) != 0) {
            fprintf(stderr, "SET_DEPTH failed (value=%u): %s\n", d, strerror(errno));
            close(fd);
            return 1;
        }
    }
    /*
    * Take the current ctl.
    * Check each flag that the user sent and update the ctl.
    * return the updated ctl to the LBR.
    */
     if (usr || ker || callstack || filter) {
        __u64 ctl = rep.lbr_config.lbr_ctl;

        
        if (usr)       ctl |= LBR_CTL_USR;
        if (ker)       ctl |= LBR_CTL_KER;
        if (callstack) ctl |= LBR_CTL_CALL_STACK;

        
        if (filter) {
            if (fbits[0]) ctl |= LBR_CTL_FILTER0;
            if (fbits[1]) ctl |= LBR_CTL_FILTER1;
            if (fbits[2]) ctl |= LBR_CTL_FILTER2;
            if (fbits[3]) ctl |= LBR_CTL_FILTER3;
            if (fbits[4]) ctl |= LBR_CTL_FILTER4;
            if (fbits[5]) ctl |= LBR_CTL_FILTER5;
            if (fbits[6]) ctl |= LBR_CTL_FILTER6;
        }

        if (ioctl(fd, LBR_IOCTL_SET_CTL, &ctl) != 0) {
            fprintf(stderr, "SET_CTL failed: %s\n", strerror(errno));
            close(fd);
            return 1;
        }
    }

    close(fd);
    return 0;
}
