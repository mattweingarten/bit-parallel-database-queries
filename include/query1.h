#ifndef QUERY1
#define QUERY1


#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void q1(uint32_t * data,uint32_t * dest,int rows,int cols);

void q1_weave(uint32_t * data,uint32_t * dest,uint32_t *temps,int word_size,int block_size,int num_features, int num_samples,int number_entries);

#endif
