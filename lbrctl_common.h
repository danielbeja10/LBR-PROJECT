#ifndef LBRCTL_COMMON_H
#define LBRCTL_COMMON_H

#include <stdio.h>
#include <stdint.h>
#include "lbr_API.h"

/* shared helpers */
/*
* Open the device and return fd
*/
int   open_dev(const char* path); 

/*
* return explanation about how to use the program.
*/
void  usage(FILE* f);


/*
* Json output of the branches.
*/
void  write_json(FILE* out, const struct lbr_entry_uapi* ents, unsigned n);


/*
* Create a 7 filter bits according to what the user chose.
*/
__u64 build_filter_bits(unsigned mask7);

#endif /* LBRCTL_COMMON_H */
