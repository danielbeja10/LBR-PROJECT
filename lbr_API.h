#pragma once


#include <linux/types.h>
#include <linux/ioctl.h>


#define LBR_IOCTL_MAGIC 'L'


/* 
 * Raw value of IA32_LBR_CTL so user space can tell which modes are ON.
 */
struct lbr_config_state {
    __u64 lbr_ctl;  /* raw IA32_LBR_CTL value as read from the CPU */
};

#define LBR_CTL_ENABLE     (1ULL << 0)  /* recording enable */
#define LBR_CTL_KER        (1ULL << 1)  /* record at CPL0 */
#define LBR_CTL_USR        (1ULL << 2)  /* record at CPL>0 */
#define LBR_CTL_CALL_STACK (1ULL << 3)  /* call-stack mode */


#define LBR_CTL_FILTER0    (1ULL << 16) // if filter is on.
#define LBR_CTL_FILTER1    (1ULL << 17) 
#define LBR_CTL_FILTER2    (1ULL << 18)
#define LBR_CTL_FILTER3    (1ULL << 19)
#define LBR_CTL_FILTER4    (1ULL << 20)
#define LBR_CTL_FILTER5    (1ULL << 21)
#define LBR_CTL_FILTER6    (1ULL << 22)

/* 
 * depth_bitmap: legal depths; bit n -> depth 8*(n+1) is supported.
 * max_depth_supported: convenience pre-computed from bitmap.
 * current_depth: the depth currently configured in IA32_LBR_DEPTH.
 */
struct lbr_limits {
    __u32 depth_bitmap;        /* legal depths (architectural LBR) */
    __u32 max_depth_supported; /* derived from bitmap (e.g., 32, 48, 64) */
    __u32 current_depth;       /* currently active depth */
};

/* 
 * this is the information we give to the client.
 */
struct lbr_basic_report {
    __u8   has_lbr;   // do we have LBR?
    struct lbr_config_state lbr_config;     /* (2) which modes are currently ON/OFF */
    struct lbr_limits       lbr_limits;     /* (3) limits, esp. max trackable depth */
};

/* 
 * GET_BASIC: kernel fills lbr_basic_report and copies to user.
 */
#define LBR_IOCTL_GET_BASIC  _IOR(LBR_IOCTL_MAGIC, 0x00, struct lbr_basic_report)
#define LBR_IOCTL_SET_CTL    _IOW(LBR_IOCTL_MAGIC, 0x01, __u64)
#define LBR_IOCTL_SET_DEPTH  _IOW(LBR_IOCTL_MAGIC, 0x02, __u32)
#define LBR_IOCTL_ENABLE     _IO (LBR_IOCTL_MAGIC, 0x03)
#define LBR_IOCTL_DISABLE    _IO (LBR_IOCTL_MAGIC, 0x04)

/* A branch record for userspace */
struct lbr_entry_uapi {
    __u64 from;
    __u64 to;
};

/* Dump request: userspace tells us where/what to copy */
struct lbr_dump_req {
    __u64 buf;      /* userspace pointer to array of struct lbr_entry_uapi */
    __u32 max;      /* IN: capacity (entries) of userspace buffer */
    __u32 count;    /* OUT: how many entries we actually copied to the user */
    __u8  clear;    /* IN: 1=reset ring after dump; 0=leave as-is */
};

#define LBR_IOCTL_DUMP_ENTRIES _IOWR(LBR_IOCTL_MAGIC, 0x05, struct lbr_dump_req)