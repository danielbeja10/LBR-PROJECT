#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/minmax.h>
#include <linux/printk.h>
#include <asm/msr.h>
#include "lbr_API.h"   /* lbr_limits definition and macros */
#include "lbr_info.h"  /* lbr_get_support, lbr_get_limits */
#include "lbr_logger.h"


/* Architectural LBR MSR bases (FROM/TO) */
#define IA32_LBR_FROM_BASE  0x1500ull // FROM MSR
#define IA32_LBR_TO_BASE    0x1600ull // TO MSR



int lbr_log_init(struct lbr_log *log)
{
     struct lbr_limits lim;
    __u8 has = 0;
    int rc;

    if (!log)
        return -EINVAL;

    rc = lbr_get_limits(&lim);// if the limits are valid in the CPU.
    if (rc != 0)
        return rc;
 
    log->entries = kcalloc(lim.max_depth, sizeof(*log->entries), GFP_KERNEL); // create space for the entries of the TO and FROM.
    if (!log->entries) // check if *log has enough space.
        return -ENOMEM;
      

    log->capacity = lim.max_depth; //the LBR can contain max depth number of branches.
    pr_info("lbr: created array of %u lbr entries"\n, log->capacity);   
    log->count = 0; // how many branches we recorded.
    return 0;
}


void lbr_log_destroy(struct lbr_log *log)
{
    if (!log)
        return;

    kfree(log->entries);
    log->entries = NULL;
    log->capacity = 0;
    log->count = 0;
}


int lbr_log_capture(struct lbr_log *log)
{
    struct lbr_limits lim;
    __u8 has = 0;
    int rc;
    u32 i, n;

    if (!log || !log->entries || log->capacity == 0)  // no recordes captured or no sturct or the capacity of the CPU is 0.
        return -EINVAL;

    rc = lbr_get_limits(&lim); // checks valid limits.
    if (rc != 0)
        return rc;

    n = min(lim.current_depth, log->capacity); // we might have CPU capacity of 32 but we only want current depth of 16.
    pr_info("capture %u LBR records\n", n);
    for (i = 0; i < n; i++) {
        u64 from = 0, to = 0;
        rdmsrl(IA32_LBR_FROM_BASE + i, from);  
        rdmsrl(IA32_LBR_TO_BASE + i, to);    
        log->entries[i].from = from;
        log->entries[i].to   = to;
        pr_info("captured[%u]: from=%#llx to=%#llx\n",
                i, (unsigned long long)from, (unsigned long long)to);
    }

    log->count = n;
    return 0;
}

