#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "lbrctl_common.h"
#include "lbr_API.h"

static void print_depth_options(__u32 opts) {
    int first = 1;
    printf("depth_options mask: 0x%08x\n", opts);
    printf("legal depths     : ");
    for (int n = 0; n < 32; n++) {
        if (opts & (1u << n)) {
            unsigned d = 8u * (n + 1u);
            if (first == 0) 
                printf(", ");
            printf("%u", d);
            first = 0;
        }
    }
    if (first) printf("(none)");
    printf("\n");
}

static void print_ctl_bits(__u64 ctl) {
    printf("LBR_CTL raw      : 0x%016llx\n", (unsigned long long)ctl);
    printf("  ENABLE     : %s\n", (ctl & LBR_CTL_ENABLE)     ? "ON" : "off");
    printf("  USR        : %s\n", (ctl & LBR_CTL_USR)        ? "ON" : "off");
    printf("  KER        : %s\n", (ctl & LBR_CTL_KER)        ? "ON" : "off");
    printf("  CALL_STACK : %s\n", (ctl & LBR_CTL_CALL_STACK) ? "ON" : "off");

    printf("  FILTERS    :");
    int any = 0;
    if (ctl & LBR_CTL_FILTER0) { printf(" 1"); any = 1; }
    if (ctl & LBR_CTL_FILTER1) { printf(" 2"); any = 1; }
    if (ctl & LBR_CTL_FILTER2) { printf(" 3"); any = 1; }
    if (ctl & LBR_CTL_FILTER3) { printf(" 4"); any = 1; }
    if (ctl & LBR_CTL_FILTER4) { printf(" 5"); any = 1; }
    if (ctl & LBR_CTL_FILTER5) { printf(" 6"); any = 1; }
    if (ctl & LBR_CTL_FILTER6) { printf(" 7"); any = 1; }
    if (!any) printf(" (none)");
    printf("\n");
}


int cmd_status(const char* dev, int argc, char** argv) {
    const char* what = NULL;  // NULL=both, "limits", or "ctl"
    if (argc >= 1) {
        if (strcmp(argv[0], "limits") == 0 || strcmp(argv[0], "ctl") == 0) {
            what = argv[0];
        } else {
            fprintf(stderr, "status: unknown arg '%s' (use: limits|ctl)\n", argv[0]);
            return 2;
        }
    }

    int fd = open_dev(dev);
    if (fd < 0) return 1;

    struct lbr_basic_report rep = (struct lbr_basic_report){0};
    if (ioctl(fd, LBR_IOCTL_GET_BASIC, &rep) != 0) {
        fprintf(stderr, "GET_BASIC failed: %s\n", strerror(errno));
        close(fd);
        return 1;
    }

    if (!rep.has_lbr) {
        printf("LBR support: NO\n");
        close(fd);
        return 0;
    }
     printf("LBR support: YES\n");

    if (what == NULL || strcmp(what, "limits") == 0) {
        printf("\n== LIMITS ==\n");
        printf("current_depth    : %u\n", rep.lbr_limits.current_depth);
        printf("max_depth        : %u\n", rep.lbr_limits.max_depth);
        print_depth_options(rep.lbr_limits.depth_options);
    }

     if (!what || strcmp(what, "ctl") == 0) {
        printf("\n== CTL ==\n");
        print_ctl_bits(rep.lbr_config.lbr_ctl);
    }

    close(fd);
    return 0;
}