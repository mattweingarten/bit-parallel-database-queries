
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

	

	
	// validate_query(&q3_fast_recon_fast_modulo,Q3);
	// test_integer_vector_division(10000);
	// test_integer_vector_mod(1000);
	
	// validate_query(&q3_vector_v5,Q3);
	// validate_query(&q2_weave,Q2);

	// test_fast_recon(256,2,asc_gen);
	// take_perf(28000,20);
	// test_integer_vector_mod(10000);
	// test_q3()
	// test_fast_recon(256,2,rand_gen);


	// fast_recon()
	// // correct = test_q3(&q3_weave_index_vertical_block_v2,rand_gen,mod_gen,5120,4,5120,4);
	bool correct;
	correct = test_q3(&q3_fast_recon_fast_modulo,asc_gen,mod_gen,128,4,128,4);
	printf("Correct: %d\n",correct);

	// correct = test_q3(&q3_weave_index_l1_block,rand_gen,	mod_gen,1024,4,1024,4);	
	// // // correct = test_q3(&q3_vector_v5,rand_gen,mod_gen,256,4,128,4);

	// // correct = test_q3(&q3_weave_index_vertical_block_v2,rand_gen,one_zero_gen,256,4,128,4);
	// correct = test_q3(&q3_weave_index_vertical_block_v2,rand_gen,one_zero_gen,32768,4,32768,4);

	// // correct = test_q3(&q3_weave_index_vertical_block_v3,rand_gen,one_zero_gen,32768,4,32768,4);


	// perf_q3_rows("vert_block_v3,B=1",&q3_weave_index_vertical_block_v3,&one_zero_gen,32768,2,16,5120);



	// perf_q3_rows("vert_block_v1,B=1",&q3_weave_index_vertical_block_v2,&one_zero_gen,32768,2,16,5120);
	// perf_q3_rows("with_vert_block=2",&q3_weave_index_vertical_block,&one_zero_gen,32768,2,16,5120);
	// perf_q3_rows("vector_v4",&q3_vector_v5,&one_zero_gen,32768,2,16,5120);
	// perf_q3_rows("query3_v3_rand_rand_gen",&q3_vector_v3,&rand_gen,32768,2,16,5120);
	// perf_q3_rows("query3_v5_rand_100_gen",&q3_vector_v5,&rand_gen,32768,2,4,5120);

	
	// // correct = test_q3(&q3_weave_index_l1_block,rand_gen,rand_gen,256,4,256,4);
	// correct = test_q3(&q3_weave_index_vertical_block,rand_gen,asc_gen,256,4,128,4);
	// 


	//bool correct;
	
	// correct = test_q3(&q3_weave_index_l1_block,rand_gen,rand_gen,256,4,256,4);
	//correct = test_q3(&q3_vector_v4,rand_gen,rand_gen,256,4,256,4);
	//printf("Correct: %d\n",correct);
	/*



	//=========================================================================================================


	performance_rnd_query(&q1_weave, Q1, "weave_v1.csv");
	performance_rnd_query(&q1_weave_v2, Q1, "weave_v2.csv");
	performance_rnd_query(&q1_weave_v3, Q1, "weave_v3.csv");
	performance_rnd_query(&q1_weave_v4, Q1, "weave_v4.csv");
	performance_rnd_query(&q1_weave_v5, Q1, "weave_v5.csv");
	performance_rnd_query(&q1_weave_v6, Q1, "weave_v6.csv");
	//add 64 features for these ones
	
	performance_rnd_query(&q1_parallel_weave, Q1, "parallel_weave_v1.csv");
	performance_rnd_query(&q1_parallel_weave_v2, Q1, "parallel_weave_v2.csv");
	performance_rnd_query(&q1_vector_weave, Q1, "vector_weave_v1.csv");
	performance_rnd_query(&q1_vector_weave_v2, Q1, "vector_weave_v2.csv");
	performance_rnd_query(&q1_vector_weave_v3, Q1, "vector_weave_v3.csv");
	performance_rnd_query(&q1_vector_weave_v3_1, Q1, "vector_weave_v3_1.csv");
	performance_rnd_query(&q1_vector_weave_v4, Q1, "vector_weave_v4.csv");
	performance_rnd_query(&q1_vector_weave_v4_1, Q1, "vector_weave_v4_1.csv");
	performance_rnd_query(&q1_vector_weave_v5, Q1, "vector_weave_v5.csv");
	performance_rnd_query(&q1_vector_weave_v6, Q1, "vector_weave_v6.csv");
	performance_rnd_query(&q1_vector_weave_v64, Q1, "vector_weave_v64.csv");
	*/
	
	// void (*queries[6])() = {&q1_weave, &q1_weave_v2, &q1_weave_v3, &q1_weave_v4, &q1_weave_v5, &q1_weave_v6};
	
	// performance_rnd_query_v2(queries, Q1, "testout.csv", 6);
	
	//void (*queries[2])() = {&q1_weave, &q1_weave_v2};
	
	//performance_rnd_query_v2(queries, Q1, "testout.csv", 2);
	// correct = test_q3(&q3_vector_v3,mod_gen,mod_gen,1024,2,1024,4);
	// printf("Correct: %d\n",correct);
	

	//=========================================================================================================


	// perf_q3_R_rows("q3_weave_index_l1_block",&q3_weave_index_l1_block,&rand_gen,32768,1024 ,2,1024,4);
	// perf_q3_R_rows("q3_weave_index",&q3_weave_index,&rand_gen,32768,1024 ,2,1024,4);

	
	return 0;
}
