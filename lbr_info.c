#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/smp.h>
#include <linux/bitops.h>
#include <asm/msr.h>
#include <linux/printk.h>
#include <asm/processor.h>
#include "lbr_API.h"

#include "lbr_info.h"

#define LBR_LEAF     0x1C // number of the leaf that give LBR information.


#define MSR_IA32_LBR_CTL        0x14CEull //relent MSR for controlling configurations of the LBR
#define MSR_IA32_LBR_DEPTH      0x14CFull // relevnt MSR for understading the depth of the LBR (How many branches)

/*
* if the caller for this functions asked for one of the registers of the leaf, set the register information from the CPU.
*/
static void cpuid_1c(u32 *a, u32 *b, u32 *c, u32 *d){

    u32 EAX = 0, EBX = 0, ECX = 0, EDX = 0;
    cpuid_count(LBR_LEAF, 0, &EAX, &EBX, &ECX, &EDX);
    if (a != NULL) *a = EAX;
    if (b != NULL) *b = EBX;
    if (c != NULL) *c = ECX;
    if (d != NULL) *d = EDX;
}
/*
* if there is information in the register return TRUE
* else - return FALSE.
*/
static bool lbr_present(void)
{
    u32 EAX = 0, EBX = 0, ECX = 0, EDX = 0;
    cpuid_1c(&EAX, &EBX, &ECX, &EDX);
    if ((EAX | EBX | ECX | EDX) != 0)
        return true;
    else
        return false;
}
/*
* return the max depth from the valid options of the depth_options.
*/
static __u32 maxDepthBit(__u32 depth_options){
    u32 max_depth = 0;
    if (depth_options !=0)
    {
        u32 n = 0;
        for(int i = 0;i < 32; i++){
            if (((1u << i) & depth_options) != 0)
            {
                n = (u32)i;
            }
            
        }
        max_depth = 8u * (n + 1u);
    }
    return max_depth;
}

/*
* checks if the LBR leaf contain information - if it is, means that LBR is availible with the modes.
* return 0 if support.
*/
int lbr_get_support(__u8 *has_lbr_out)

{
      bool lbrFlag;

    if (has_lbr_out == NULL)
        return -EINVAL;

    lbrFlag = lbr_present();
    if (lbrFlag) {
        *has_lbr_out = 1;
        pr_info("lbr: LBR is available.\n");
        return 0;
        
    } else {
        *has_lbr_out = 0;
        pr_err("lbr: LBR is NOT available.\n");
        return -ENODEV;
    }
}

/*
* return the ctl state with all the configuration information of the LBR.
*/
int lbr_get_config_state(struct lbr_config_state *out)
{
    __u8 has_lbr = 0;
    u64 ctl = 0;
    int rc;

    if (out == NULL)
        return -EINVAL;

    rc = lbr_get_support(&has_lbr);
    if (rc != 0)
        return rc;

    rdmsrl(MSR_IA32_LBR_CTL, ctl);
    out->lbr_ctl = ctl;
    pr_info("lbr: the LBR current configuration state is 0x%llx.\n", (unsigned long long)ctl);
    return 0;
}


int lbr_get_limits(struct lbr_limits *out)
{
    __u8 has_lbr = 0;
    u32 EAX = 0, EBX = 0, ECX = 0, EDX = 0;
    u32 depth_opt = 0;
    u64 depth_msr = 0;
    int rc;

    if (out == NULL)
        return -EINVAL;

    rc = lbr_get_support(&has_lbr);
    if (rc != 0)
        return rc;

    cpuid_1c(&EAX, &EBX, &ECX, &EDX); // take info from the leaf
    depth_opt = EAX & 0xFFu; // what leagal depth does the cpu has.
   

    rdmsrl(MSR_IA32_LBR_DEPTH, depth_msr); // what is the current depth that the LBR is using.

    out->depth_options = depth_opt;
    pr_info("lbr: the LBR depth options are %#x\n", depth_opt);
    out->current_depth = (u32)depth_msr;
    pr_info("lbr: the LBR current depth is %#x\n", out->current_depth);
    out->max_depth = maxDepthBit(depth_opt);
    pr_info("lbr: the LBR MAX depth is %#x\n", out->max_depth);

    if (out->depth_options == 0)
        return -ENODEV;
    else
        return 0;
}
