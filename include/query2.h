#ifndef QUERY2
#define QUERY2

#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


uint64_t q2(uint32_t * data,int rows,int cols);

uint64_t q2_weave(uint32_t * data,uint32_t * cond_buffer,uint32_t *temp_buffer,uint32_t *sum_buffer,int word_size,int block_size,int num_samples,int num_features,int number_entries);

uint64_t q2_weave_v2(uint32_t * data,uint32_t * cond_buffer,uint32_t *temp_buffer,uint32_t *sum_buffer,int word_size,int block_size,int num_samples,int num_features,int number_entries);

uint64_t q2_weave_scalar(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size, int num_samples,int num_features,int number_entries);

uint64_t q2_weave_scalar_vector_accum(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size, int num_samples,int num_features,int number_entries);

uint64_t q2_weave_unroll(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size, int num_samples,int num_features,int number_entries);

uint64_t q2_weave_unroll_vector_accum(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size, int num_samples,int num_features,int number_entries);

uint64_t q2_vector_weave(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size,int num_samples, int num_features,int number_entries);

uint64_t q2_vector_weave_2(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size,int num_samples, int num_features,int number_entries);


#endif
