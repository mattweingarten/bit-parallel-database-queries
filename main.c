
#include "include/load.h"
#include "include/converter.h"
#include "include/debug.h"
#include "include/query1.h"
#include "include/query2.h"
#include "include/query3.h"
#include "include/query.h"
#include "include/unit_tests.h"
#include "include/validate.h"
#include "include/perform.h"
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>  
#include <assert.h>


int main(int argc, char **argv) {
	// const uint32_t max = 1L << 31;
	// // __m256i x_v = _mm256_setr_epi32(1,2,3,4,5,6,7,8);
	// __m256i x_v = _mm256_set1_epi32(max);
	// // PRINT_32_BIT_VECTOR(x_v);
	// __m256i m_v = _mm256_set1_epi32(2);
	// // PRINT_32_BIT_VECTOR(m_v);
	// __m256i t_lo = _mm256_mul_epu32(x_v,m_v);
	// t_lo = _mm256_srli_epi64(t_lo,32);
	// __m256i t_hi = _mm256_srli_epi64(x_v,32);
	// t_hi = _mm256_mul_epu32(t_hi,m_v);
	// // PRINT_32_BIT_VECTOR(t_hi);
	// // t_lo = _mm256_mul_epu32(m_v,t_hi);
	// // PRINT_32_BIT_VECTOR(t_lo);
	// PRINT_32_BIT_VECTOR(t_lo);
	// PRINT_32_BIT_VECTOR(t_hi);

	// // __m256i t = _mm256_unpackhi_epi32(t_lo,t_hi);
	// __m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);

	// __m256i res = _mm256_sub_epi32(x_v,t);
	// res = _mm256_srli_epi32(res,1);
	// res = _mm256_add_epi32(res,t);
	// res = _mm256_srli_epi32(res,0);
	// PRINT_32_BIT_VECTOR(t);
	
	test_integer_vector_division(10000);

	
	// validate_query(&q3_vector_v2,Q3);
	
	
	// bool correct;

	// test_integer_vector_division(10000);

	// test_fast_uint_mod(100);
	// test_fast_uint_div(1000);
	// fast_uint_div(100, 8);

	// test_q3(&q3_unroll,mod_gen,mod_gen,128,4,128,4);
	// test_q3(&q3_unroll_v2,mod_gen,mod_gen,128,4,128,4);
	// bool correct = test_q3(&q3_weave_index_l1_block,first_col_gen,mod_gen,64,8,128,4);
	// printf("Correct: %d\n",correct);
	// correct = test_q3(&q3_vector,j_gen,j_gen,512,16,128,8);
	// printf("Correct: %d\n",correct);
	// validate_query(&q3_weave_index_l2_block,Q3b);
	// perf_q3_compare_block("q3_blocking_measurements",&q3_weave_index_l2_block,16384);


	// perf_q3_R_rows("q3_weave_index_l1_block",&q3_weave_index_l1_block,&rand_gen,32768,1024 ,2,1024,4);
	// perf_q3_R_rows("q3_weave_index",&q3_weave_index,&rand_gen,32768,1024 ,2,1024,4);

	
	return 0;
}
