#ifndef QUERY3
#define QUERY3

#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


uint32_t cartesian_product_size(int R_rows, int R_cols, int S_rows, int S_cols);


void q3(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * output_size, int R_rows, int R_cols, int S_rows, int S_cols);



#endif
