#pragma once
#include <linux/types.h>
#include "lbr_API.h" 

/*
* checks if LBR is availible on the CPU.
* return 0 if yes, -ENODEV if not.
*/
int lbr_get_support(__u8 *has_lbr_out);


/*
* Read IA32_LBR_CTL and store raw value into 'out->lbr_ctl'.
* Returns 0 on success, -ENODEV if not.
*/

int  lbr_get_config_state(struct lbr_config_state *out);

/*
* Provide legal depths (bitmap), max legal depth, and the currently configured depth.
* Returns 0 on success, -ENODEV if not.
*/
int  lbr_get_limits(struct lbr_limits *out);

