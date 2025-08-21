#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "lbr_logger.h"

#include "lbr_API.h"     /* IOCTL numbers + UAPI structs */
#include "lbr_info.h"    /* lbr_get_support, lbr_get_config_state, lbr_get_limits */
#include "lbr_control.h" /* lbr_enable, lbr_disable, lbr_set_ctl, lbr_set_depth */



/*
 * ioctl handler for LBR device
 * This function receives commands from user space and calls
 * the appropriate kernel-side LBR routines.
 */
static long lbr_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    long rc = 0;

    switch (cmd) {

    case LBR_IOCTL_GET_BASIC: {
        /* 
        * Collect basic LBR information for the user:
        * - Check if LBR is supported
        * - Get the current configuration
        * - Get LBR limits
        * Then copy the full report to user space.
        */
        struct lbr_basic_report rep;
        __u8 has = 0;

        rc = lbr_get_support(&has); // check hardware LBR support
        if (rc != 0)
            return rc;

        rep.has_lbr = has;

        rc = lbr_get_config_state(&rep.lbr_config); // get current config
        if (rc != 0)
            return rc;

        rc = lbr_get_limits(&rep.lbr_limits); // get limits info
        if (rc != 0)
            return rc;

        if (copy_to_user((void __user *)arg, &rep, sizeof(rep)))
            return -EFAULT;

        return 0;
    }

    case LBR_IOCTL_SET_CTL: {
        /* 
        * Update the MSR LBR_CTL register with a new value from user space.
        */
        __u64 new_ctl;
        if (copy_from_user(&new_ctl, (void __user *)arg, sizeof(new_ctl)))
            return -EFAULT;
        return lbr_set_ctl(new_ctl);
    }
    
    case LBR_IOCTL_SET_DEPTH: {
        /* 
        * Change the current LBR stack depth according to user input.
        */
        __u32 depth;
        if (copy_from_user(&depth, (void __user *)arg, sizeof(depth)))
            return -EFAULT;
        return lbr_set_depth(depth);
    }

    case LBR_IOCTL_ENABLE:
        /* 
        * Enable LBR capturing on the CPU.
        */
        return lbr_enable();

    case LBR_IOCTL_DISABLE:
        /* 
        * Disable LBR capturing on the CPU.
        */
        return lbr_disable();

    default:
        /* Unsupported ioctl command. */
        return -ENOTTY;

    case LBR_IOCTL_REQ_ENTRIES: { 
        /* 
        * return the recorded LBR entries to user space:
        * - Initialize logging
        * - Capture current LBR entries
        * - Copy them to user buffer (up to req.max)
        * - Optionally clear the hardware LBR after copying
        */
        struct lbr_req req;
        struct lbr_log log;
        int rc;

        if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
            return -EFAULT;

        if (req.max == 0 || req.buf == 0)
            return -EINVAL;

        rc = lbr_log_init(&log); // prepare log buffer
        if (rc != 0)
            return rc;

        rc = lbr_log_capture(&log); // capture branches from CPU
        if (rc == 0) {
            __u32 to_copy = (log.count < req.max) ? log.count : req.max; 

            if (copy_to_user((void __user *)(uintptr_t)req.buf, log.entries, to_copy * sizeof(struct lbr_entry)))
                rc = -EFAULT;
            else
                req.count = to_copy;
        }

        if (rc == 0) {
            if (copy_to_user((void __user *)arg, &req, sizeof(req)))
                rc = -EFAULT;
        }

        if (rc == 0 && req.clear) {
            (void)lbr_disable();
            (void)lbr_enable();
        }


        lbr_log_destroy(&log);
        return rc;
    }
    }
}

/* 
 * File operations for the LBR device.
 * Only unlocked_ioctl is used for user-kernel communication here.
 */
static const struct file_operations lbr_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = lbr_ioctl,
};

/* 
 * LBR misc device registration
 * Creates /dev/lbr_device with dynamic minor number.
 */
static struct miscdevice lbr_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = "lbr_device",
    .fops  = &lbr_fops,
};

/*
 * Module init: register the LBR misc device.
 */
static int __init lbr_dev_init(void)
{
    int rc = misc_register(&lbr_miscdev);
    if (rc)
        pr_err("lbr: misc_register failed (%d)\n", rc);
    else
        pr_info("lbr: /dev/%s ready\n", "lbr_device");
    return rc;
}

/*
 * Module exit: deregister the LBR misc device.
 */
static void __exit lbr_dev_exit(void)
{
    misc_deregister(&lbr_miscdev);
    pr_info("lbr: /dev/%s removed\n", "lbr_device");
}

module_init(lbr_dev_init);
module_exit(lbr_dev_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("LBR interface (ioctl)");
