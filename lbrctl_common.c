#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#include "lbr_API.h"
#include "lbrctl_common.h"

/* device default path (internal) */
static const char* DEV_DEFAULT = "/dev/lbr_device";

/* open the device */
int open_dev(const char* path) {
    int fd = open(path ? path : DEV_DEFAULT, O_RDWR | O_CLOEXEC);
    if (fd < 0)
        fprintf(stderr, "open(%s) failed: %s\n", path ? path : DEV_DEFAULT, strerror(errno));
    return fd;
}

void usage(FILE* f) {
    fprintf(f,
"Usage:\n"
"  lbrctl config [--dev PATH] [depth N] [usr] [ker] [callstack]\n"
"                [filter1] [filter2] [filter3] [filter4] [filter5] [filter6] [filter7]\n"
"\n"
"  lbrctl run    [--dev PATH] -- <program> [args...]\n"
"  lbrctl status [--dev PATH] [limits|ctl]\n"
"\n"
"Notes:\n"
"  • depth N    -  change the LBR depth to N.\n"
"  • usr/ker     - enable the respective CTL bits (without clearing others).\n"
"  • callstack    - enable call-stack mode.\n"
"  • filter1..7  - enable filters 1..7 (mapped to FILTER0..FILTER6) without clearing others.\n"
"  • --dev PATH  - (optional) use a different device path instead of /dev/lbr_device.\n"
"\n"
"Examples:\n"
"  lbrctl config depth 16 usr callstack filter1 filter3 filter7\n"
"  lbrctl run -- ./myprog arg1 arg2\n"
"  lbrctl status\n"
"  lbrctl status limits\n"
"  lbrctl status ctl\n"
    );
}


/* print JSON */
void write_json(FILE* out, const struct lbr_entry_api* ents, unsigned n) {
    time_t now = time(NULL);
    fprintf(out, "{\n  \"generated_at\": %lld,\n  \"entries\": [\n", (long long)now);
    for (unsigned i = 0; i < n; ++i) {
        fprintf(out, "    {\"index\": %u, \"from\": \"0x%016llx\", \"to\": \"0x%016llx\"}%s\n",
                i, (unsigned long long)ents[i].from, 
                (unsigned long long)ents[i].to,
                (i + 1 == n) ? "" : ",");
    }
    fprintf(out, "  ]\n}\n");
}
