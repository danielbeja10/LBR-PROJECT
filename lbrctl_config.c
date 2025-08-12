#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <getopt.h>

#include "lbr_API.h"
#include "lbrctl_common.h"

/* 
* Create the configuration of the user.
* Without enabling LBR and without running the desired application.
*/
int cmd_config(const char* dev, int argc, char** argv) {
    int set_depth = -1;
    int have_usrker = 0, usr = 0, ker = 0;
    int callstack_set = 0, callstack_on = 0;
    int have_filter = 0, clear_filters = 0;
    unsigned filter_mask = 0;


    /*
    * Set the flags according to the input of the user.
    */
    static struct option opts[] = {
        {"depth",         required_argument, 0, 'd'},
        {"usr-only",      no_argument,       0, 1001},
        {"ker-only",      no_argument,       0, 1002},
        {"both",          no_argument,       0, 1003},
        {"callstack",     no_argument,       0, 1004},
        {"no-callstack",  no_argument,       0, 1005},
        {"filter-mask",   required_argument, 0, 2001},
        {"clear-filters", no_argument,       0, 2002},
        {0,0,0,0}
    };

    int c, idx = 0;
    optind = 1; /* reset for subcommand */
    while ((c = getopt_long(argc, argv, "d:", opts, &idx)) != -1) {
        switch (c) {
            case 'd':   set_depth = (int)strtoul(optarg, NULL, 0); break;
            case 1001:  have_usrker = 1; usr = 1; ker = 0; break;
            case 1002:  have_usrker = 1; usr = 0; ker = 1; break;
            case 1003:  have_usrker = 1; usr = 1; ker = 1; break;
            case 1004:  callstack_set = 1; callstack_on = 1; break;
            case 1005:  callstack_set = 1; callstack_on = 0; break;
            case 2001:  have_filter = 1; filter_mask = (unsigned)strtoul(optarg, NULL, 0) & 0x7F; break;
            case 2002:  have_filter = 1; clear_filters = 1; filter_mask = 0; break;
            default:    usage(stderr); return 2;
        }
    }

    /*
    * Checking LBR is availible
    */
    int fd = open_dev(dev);
    if (fd < 0) return 1;

    struct lbr_basic_report rep = (struct lbr_basic_report){0};
    if (ioctl(fd, LBR_IOCTL_GET_BASIC, &rep) != 0)
         { 
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

             fprintf(stderr, "SET_DEPTH(%u) failed: %s\n", d, strerror(errno)); 
             close(fd); 
             return 1; 
            }
    }

    /*
    * Take the current ctl.
    * Check each flag that the user sent and update the ctl.
    * return the updated ctl to the LBR.
    */
    if (have_usrker || callstack_set || have_filter) {
        __u64 ctl = rep.lbr_config.lbr_ctl; /* start from current */
        if (have_usrker) {
            ctl &= ~(LBR_CTL_USR | LBR_CTL_KER);
            if (usr) ctl |= LBR_CTL_USR;
            if (ker) ctl |= LBR_CTL_KER;
        }
        if (callstack_set) {
            if (callstack_on) ctl |= LBR_CTL_CALL_STACK;
            else              ctl &= ~LBR_CTL_CALL_STACK;
        }
        if (have_filter) {
            __u64 all = LBR_CTL_FILTER0 | LBR_CTL_FILTER1 | LBR_CTL_FILTER2 |
                        LBR_CTL_FILTER3 | LBR_CTL_FILTER4 | LBR_CTL_FILTER5 | LBR_CTL_FILTER6;
            ctl &= ~all; /* clear filter field */
            if (!clear_filters) 
                ctl |= build_filter_bits(filter_mask);
        }

        if (ioctl(fd, LBR_IOCTL_SET_CTL, &ctl) != 0) 
        { 
            fprintf(stderr, "SET_CTL failed: %s\n", strerror(errno)); 
            close(fd); 
            return 1; 
        }
    }

    
    close(fd);
    return 0;
}