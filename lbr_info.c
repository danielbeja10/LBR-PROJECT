#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/smp.h>
#include <linux/bitops.h>
#include <asm/msr.h>
#include <asm/processor.h>
#include "lbr_API.h"
#include "lbr_info.h"

#define ARCH_LBR_LEAF     0x1C // number of the leaf that give LBR information.


#define MSR_IA32_LBR_CTL        0x14CEull //relent MSR for controlling configurations of the LBR
#define MSR_IA32_LBR_DEPTH      0x14CFull // relevnt MSR for understading the depth of the LBR (How many branches)

/*
* if the caller for this functions asked for one of the registers of the leaf, set the register information from the CPU.
*/
static void cpuid_1c(u32 *a, u32 *b, u32 *c, u32 *d){

    u32 EAX = 0, EBX = 0, ECX = 0, EDX = 0;
    cpuid_count(ARCH_LBR_LEAF, 0, &EAX, &EBX, &ECX, &EDX);
    if (a) *a = EAX;
    if (b) *b = EBX;
    if (c) *c = ECX;
    if (d) *d = EDX;
}
/*
* if there is information in the register return TRUE
* else - return FALSE.
*/
static bool arch_lbr_present(void)
{
    u32 EAX = 0, EBX = 0, ECX = 0, EDX = 0;
    cpuid_1c(&EAX, &EBX, &ECX, &EDX);
    if ((EAX | EBX | ECX | EDX) != 0)
        return true;
    else
        return false;
}
/*
* calculate the max depth. 
* 8 * (n + 1) - n is the index of the most significant bit.
*/
static u32 bitmap_to_max_depth(u32 bits)
{
    int msb;
    if (bits == 0)
        return 0;
    msb = fls(bits) - 1;
    return 8u * (msb + 1);
}

/*
* checks if the LBR leaf contain information - if it is, means that LBR is availible with the modes.
* return 0 if support.
*/
int lbr_get_support(__u8 *has_lbr_out)

{
      bool arch;

    if (has_lbr_out == NULL)
        return -EINVAL;

    arch = arch_lbr_present();
    if (arch) {
        *has_lbr_out = 1;
        return 0;
    } else {
        *has_lbr_out = 0;
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
    return 0;
}


int lbr_get_limits(struct lbr_limits *out)
{
    __u8 has_lbr = 0;
    u32 EAX = 0, EBX = 0, ECX = 0, EDX = 0;
    u32 depth_bitmap = 0;
    u32 max_depth = 0;
    u64 depth_msr = 0;
    int rc;

    if (out == NULL)
        return -EINVAL;

    rc = lbr_get_support(&has_lbr);
    if (rc != 0)
        return rc;

    cpuid_1c(&EAX, &EBX, &ECX, &EDX); // take info from the leaf
    depth_bitmap = EAX & 0xFFu; // what leagal depth does the cpu has.
    max_depth = bitmap_to_max_depth(depth_bitmap); // what is the max depth it has

    rdmsrl(MSR_IA32_LBR_DEPTH, depth_msr); // what is the current depth that the LBR is using.

    out->depth_bitmap = depth_bitmap;
    out->max_depth_supported = max_depth;
    out->current_depth = (u32)depth_msr;

    if (max_depth == 0 && out->current_depth == 0) // no LBR
        return -ENODEV;
    else
        return 0;
}
