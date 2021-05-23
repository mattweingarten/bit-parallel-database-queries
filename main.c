
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

	
	// test_integer_vector_division(10000);
	// test_integer_vector_mod(1000);
	
	// validate_query(&q3_vector_v4,Q3);




	// perf_q3_rows("query_3_before_vector_rand_gen",&q3_weave_index_l1_block,&one_zero_gen,32768,2,16,5120);
	// perf_q3_rows("query3_v3_rand_rand_gen",&q3_vector_v3,&rand_gen,32768,2,16,5120);
	// perf_q3_rows("query3_v4_rand_rand_gen",&q3_vector_v4,&rand_gen,32768,2,16,5120);

	
	bool correct;
	
	// correct = test_q3(&q3_weave_index_l1_block,rand_gen,rand_gen,256,4,256,4);
	correct = test_q3(&q3_vector_v4,rand_gen,rand_gen,256,4,256,4);
	printf("Correct: %d\n",correct);


	// correct = test_q3(&q3_vector_v3,mod_gen,mod_gen,1024,2,1024,4);
	// printf("Correct: %d\n",correct);
	
	// validate_query(&q3_weave_index_l2_block,Q3b);

	// test_integer_vector_division(10000);

	// test_fast_uint_mod(100);
	// test_fast_uint_div(1000);
	// fast_uint_div(100, 8);

	// test_q3(&q3_unroll_v2,mod_gen,mod_gen,128,4,128,4);
	// bool correct = test_q3(&q3_weave_index_l1_block,first_col_gen,mod_gen,64,8,128,4);
	// correct = test_q3(&q3_vector,j_gen,j_gen,512,16,128,8);
	// printf("Correct: %d\n",correct);
	// perf_q3_compare_block("q3_blocking_measurements",&q3_weave_index_l2_block,16384);


	// perf_q3_R_rows("q3_weave_index_l1_block",&q3_weave_index_l1_block,&rand_gen,32768,1024 ,2,1024,4);
	// perf_q3_R_rows("q3_weave_index",&q3_weave_index,&rand_gen,32768,1024 ,2,1024,4);

	
	return 0;
}
