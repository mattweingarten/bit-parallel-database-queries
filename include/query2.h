#ifndef QUERY2
#define QUERY2

#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


uint64_t q2(uint32_t * data,int rows,int cols);

uint64_t q2_weave(uint32_t * data,uint32_t * cond_buffer,uint32_t *temp_buffer,uint32_t *sum_buffer,int word_size,int block_size,int num_samples,int num_features,int number_entries);

#endif
