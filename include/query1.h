#ifndef QUERY1
#define QUERY1


#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void q1(uint32_t * data,uint32_t * dest,int rows,int cols);

void q1_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples,int num_features ,int number_entries);

void q1_weave_v2(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples,int num_features ,int number_entries);

void q1_weave_v3(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples,int num_features ,int number_entries);

void q1_weave_v4(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples,int num_features ,int number_entries);

void q1_weave_v5(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples,int num_features ,int number_entries);

void q1_weave_v6(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples,int num_features ,int number_entries);

void q1_parallel_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples,int num_features ,int number_entries);

void q1_parallel_weave_v2(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples,int num_features ,int number_entries);

void q1_parallel_weave_v3(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples,int num_features ,int number_entries);

void q1_vector(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_alt_store(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_v2(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_unroll2(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_unroll4(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_unroll8(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_unroll16(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_unroll4_2xread(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_0line(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_0line_testz(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_0check(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_0check_fine(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_0check_u2(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_0check_u4(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_xorcheck_u4(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_early(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_early_0(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_early_0_u4(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);

void q1_vector_testc(uint32_t * data,uint32_t * results,uint32_t *temps,int64_t *comp_cycles,int64_t *res_cycles,int num_samples, int num_features,int number_entries);
#endif
