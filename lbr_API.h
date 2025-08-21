#pragma once


#include <linux/types.h>
#include <linux/ioctl.h>


#define LBR_IOCTL_MAGIC 'L'




#define LBR_CTL_ENABLE     (1ULL << 0)  /*  enable */
#define LBR_CTL_KER        (1ULL << 1)  /*  kernel mode */
#define LBR_CTL_USR        (1ULL << 2)  /* user mode */
#define LBR_CTL_CALL_STACK (1ULL << 3)  /* call-stack mode */


#define LBR_CTL_FILTER0    (1ULL << 16) 
#define LBR_CTL_FILTER1    (1ULL << 17) 
#define LBR_CTL_FILTER2    (1ULL << 18)
#define LBR_CTL_FILTER3    (1ULL << 19)
#define LBR_CTL_FILTER4    (1ULL << 20)
#define LBR_CTL_FILTER5    (1ULL << 21)
#define LBR_CTL_FILTER6    (1ULL << 22)


/* 
 * Raw value of IA32_LBR_CTL so user space can tell which modes are ON.
 */
struct lbr_config_state {
    __u64 lbr_ctl;  /* raw IA32_LBR_CTL value as read from the CPU */
};

/* 
 * depth_options: legal depths; bit n -> depth 8*(n+1) is supported. 
 * current_depth: the depth currently configured in IA32_LBR_DEPTH.
 */
struct lbr_limits {
    __u32 depth_options;        
    __u32 current_depth;   
    __u32 max_depth;    
};

/* 
 * this is the information we give to the client.
 */
struct lbr_basic_report {
    __u8   has_lbr;   // if 1 has LBR else doesn't.
    struct lbr_config_state lbr_config;     /* (2) which modes are currently ON/OFF */
    struct lbr_limits       lbr_limits;     /* (3) limits, esp. max trackable depth */
};

/* A branch record for userspace */
struct lbr_entry {
    __u64 from;
    __u64 to;
};

/* request: userspace tells us where/what to copy */
struct lbr_req {
    __u64 buf;      /* userspace pointer to array of struct lbr_entry */
    __u32 max;      /* IN: capacity (entries) of userspace buffer */
    __u32 count;    /* OUT: how many entries we actually copied to the user */
    __u8  clear;    /* IN: 1=reset the LBR so we can get only the new branches ; 0=leave as and get the brancehs of all the LBR-is */
};



#define LBR_IOCTL_GET_BASIC  _IOR(LBR_IOCTL_MAGIC, 0x00, struct lbr_basic_report)
#define LBR_IOCTL_SET_CTL    _IOW(LBR_IOCTL_MAGIC, 0x01, __u64)
#define LBR_IOCTL_SET_DEPTH  _IOW(LBR_IOCTL_MAGIC, 0x02, __u32)
#define LBR_IOCTL_ENABLE     _IO (LBR_IOCTL_MAGIC, 0x03)
#define LBR_IOCTL_DISABLE    _IO (LBR_IOCTL_MAGIC, 0x04)
#define LBR_IOCTL_REQ_ENTRIES _IOWR(LBR_IOCTL_MAGIC, 0x05, struct lbr_req)

