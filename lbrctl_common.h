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
void write_json(FILE* out, const struct lbr_entry* ents, unsigned n);


