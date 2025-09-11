#pragma once
#include <linux/types.h>
#include "lbr_API.h" 


/* In-memory buffer to hold a snapshot of LBR entries */
struct lbr_log {
    struct lbr_entry *entries; /* allocated array */
    u32 capacity;              /*  array size (in entries) max depth */
    u32 count;                 /* how many entries captured last time */
};

/* Allocate internal storage sized to the CPU's max LBR depth */
int  lbr_log_init(struct lbr_log *log);

/* Free storage */
void lbr_log_destroy(struct lbr_log *log);

/* Read current LBR entries into 'log->entries' and set 'log->count' */
int  lbr_log_capture(struct lbr_log *log);

