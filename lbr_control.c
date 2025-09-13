#include <linux/errno.h>
#include <asm/msr.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <asm/processor.h>
#include "lbr_API.h"
#include "lbr_info.h"

#define MSR_IA32_LBR_CTL    0x14CEull
#define MSR_IA32_LBR_DEPTH  0x14CFull

// check if the new mode state we want to set is valid.
static int validate_ctl_bits(u64 new_ctl)
{
    u64 allowed = 0;

    allowed |= LBR_CTL_ENABLE;
    allowed |= LBR_CTL_KER | LBR_CTL_USR | LBR_CTL_CALL_STACK;
    allowed |= (LBR_CTL_FILTER0 | LBR_CTL_FILTER1 | LBR_CTL_FILTER2 |
                LBR_CTL_FILTER3 | LBR_CTL_FILTER4 | LBR_CTL_FILTER5 | LBR_CTL_FILTER6);

    if ((new_ctl & ~allowed) != 0) // check if we change only bits that are related to the modes(ctl).
        return -EINVAL;

    return 0;
}
//check if the depth we want to set is valid.
static int validate_depth_value(u32 new_depth)
{
    struct lbr_limits lim;
    int rc;

    if (new_depth == 0) 
        return -EINVAL;
    if ((new_depth % 8) != 0) // not valide depth 
        return -EINVAL;

    rc = lbr_get_limits(&lim);
    if (rc != 0) // some error within the limit check
        return rc;

    {
        u32 n = (new_depth / 8) - 1; // set the bit of the new depth.
        if (n >= 32) // cant be bigger than 32.
            return -EINVAL;
        if ((lim.depth_options & (1u << n)) == 0) // if the depth that we are trying to change isnt leagal for the cpu return false.
            return -EINVAL;
    }
    return 0;
}

int lbr_enable(void)
{
    __u8 has = 0;
    u64 ctl;
    int cpu = get_cpu();
    put_cpu();

    pr_info("lbr: ENABLE requested by pid=%d (comm=%s) on cpu=%d\n",
            current->pid, current->comm, cpu);

    
    if (lbr_get_support(&has) != 0) //if there is no LBR.
        return -ENODEV;

    rdmsrl(MSR_IA32_LBR_CTL, ctl);
    ctl |= LBR_CTL_ENABLE;
    wrmsrl(MSR_IA32_LBR_CTL, ctl); // return the same LBR CTL just with enable bit[0] as 1.
    pr_info("lbr: ENABLE done\n");
    return 0;
}



int lbr_disable(void)
{
    __u8 has = 0;
    u64 ctl;
    int cpu = get_cpu();
    put_cpu();

    pr_info("lbr: DISABLE requested by pid=%d (comm=%s) on cpu=%d\n",
            current->pid, current->comm, cpu);


    if (lbr_get_support(&has) != 0) // if there is no LBR.
        return -ENODEV;

    rdmsrl(MSR_IA32_LBR_CTL, ctl); 
    ctl &= ~LBR_CTL_ENABLE;
    wrmsrl(MSR_IA32_LBR_CTL, ctl); // return the same LBR CTL just with enable bit[0] as 0.
    pr_info("lbr: DISABLE done\n");
    return 0;
}


int lbr_set_ctl(u64 new_ctl)
{
    __u8 has = 0;
    int rc;
    pr_info("lbr: SET_CTL 0x%llx (pid=%d)\n",
            (unsigned long long)new_ctl, current->pid);


    if (lbr_get_support(&has) != 0) // if there is no LBR.
        return -ENODEV;

    rc = validate_ctl_bits(new_ctl); //check if the new ctl is valid
    if (rc != 0)
        return rc;

    wrmsrl(MSR_IA32_LBR_CTL, new_ctl);
    pr_info("lbr: SET_CTL to 0x%llx done\n",(unsigned long long)new_ctl);
    return 0;
}


int lbr_set_depth(u32 new_depth)
{
    __u8 has = 0;
    int rc;
    pr_info("lbr: SET_DEPTH %u (pid=%d)\n", new_depth, current->pid);

    if (lbr_get_support(&has) != 0) // if there is no LBR.
        return -ENODEV;

    rc = validate_depth_value(new_depth); 
    if (rc != 0)
        return rc;

    wrmsrl(MSR_IA32_LBR_DEPTH, (u64)new_depth);
    pr_info("lbr: SET_DEPTH to 0x%x done\n",new_depth);
    return 0;
}
