#pragma once
#include <linux/types.h>
#include "lbr_API.h"   /* LBR_CTL_* masks, structs */
#include "lbr_info.h"  /* lbr_get_support, lbr_get_limits, lbr_get_config_state */

/* Current-CPU control API */
int lbr_enable(void); // Enable the LBR.
int lbr_disable(void); // Disable the LBR.
int lbr_set_ctl(u64 new_ctl); // Set the new mode for the LBR.
int lbr_set_depth(u32 new_depth); // Set new depth for the LBR.
