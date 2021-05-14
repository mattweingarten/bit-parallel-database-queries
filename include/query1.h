#ifndef QUERY1
#define QUERY1


#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void q1(uint32_t * data,uint32_t * dest,int rows,int cols);

void q1_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries);

void q1_weave_v2(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries);

void q1_weave_v3(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries);

void q1_weave_v4(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries);

void q1_weave_v5(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries);

void q1_weave_v6(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries);

void q1_parallel_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries);

void q1_vector_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples, int num_features,int number_entries);

#endif
