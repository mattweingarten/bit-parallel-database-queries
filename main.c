
#include "include/load.h"
#include "include/converter.h"
#include "include/debug.h"
#include "include/query1.h"
#include "include/query2.h"
#include "include/query3.h"
#include "include/query3_archive.h"
#include "include/query.h"
#include "include/generator.h"
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
	// srand(time(NULL));

	// fast_recon_perf();

	// test_q3(&q3_weave_index_l1_block,rand_gen,rand_gen,5120,4,5120,4);
	// test_q3(&q3_vector_v5,rand_gen,rand_gen,5120,2,5120,4);
	// test_q3(&q3_fast_recon_fast_modulo,rand_gen,rand_gen,5120,4,5120,4);

	// test_q3(&q3_weave_index_l1_block,rand_gen,rand_gen,5120,4,5120,4);
	// test_q3(&q3_vector_v5,rand_gen,rand_gen,5120,2,5120,4);
	// test_q3(&q3_fast_recon_fast_modulo,rand_gen,rand_gen,5120,4,5120,4);

	// perf_q3_selectiviy("selectiviy_fast_recon_O2",&q3_fast_recon_fast_modulo,8192,4);

	// perf_q3_selectiviy("selectiviy_vector_v5_O2",&q3_vector_v5,8192,4);
	validate_query(&q3_fast_recon_fast_modulo,Q3);
	// uint32_t * S;
	// uint32_t * R;

	// generate_selective_db(1024,4,0.8,&S,&R);
	
	
	// // PRINT_MALLOC(S,1024,4);
	// uint32_t gt_out_size = cart_prod(1024,1024);
	// uint32_t* gt = (uint32_t*) aligned_alloc( 32, gt_out_size * 2 * sizeof(uint32_t));
	// q3_index(R,S,gt,&gt_out_size,1024,4,1024,4);
	// uint32_t* re_gt = realloc(gt,gt_out_size * 2 *  sizeof(uint32_t));
	
	// perf_q3_rows("early_prune,R_cols=32",&q3_with_prune,&rand_gen,20480,2,16,5120);
	// perf_q3_rows("straight_2_4_rand",&q3_weave_index,&rand_gen,10240,2,4,5120);
	// perf_q3_rows("block_2_4_rand",&q3_weave_index_l1_block,&rand_gen,30720,2,4,5120);
	// perf_q3_rows("fast_mod_2_4_rand",&q3_vector_v5,&rand_gen,30720,2,4,5120);
	// perf_q3_rows("fast_recon_2_4_rand",&q3_fast_recon_fast_modulo,&rand_gen,30720,2,4,5120);


	// perf_q3_rows("straight_32_rand",&q3_weave_index,&rand_gen,20480,32,32,5120);
	// perf_q3_rows("block_32_rand",&q3_weave_index_l1_block,&rand_gen,30720,32,32,5120);
	// perf_q3_rows("fast_mod_32_rand",&q3_vector_v5,&rand_gen,30720,32,32,5120);
	// perf_q3_rows("fast_recon_32_rand",&q3_fast_recon_fast_modulo,&rand_gen,30720,32,32,5120);



	// perf_q3_rows("early_prune",&q3_weave_index,&rand_1000_gen,20480,2,4,5120);
	// perf_q3_rows("early_prune",&q3_weave_index_l1_block,&rand_1000_gen,30720,2,4,5120);
	// perf_q3_rows("early_prune",&q3_vector_v5,&rand_1000_gen,30720,2,4,5120);
	// perf_q3_rows("early_prune",&q3_fast_recon_fast_modulo,&rand_1000_gen,30720,2,4,5120);


	// perf_q3_rows("early_prune",&q3_fast_recon_fast_modulo,&rand_100_gen,20480,2,16,5120);
	// perf_q3_rows("early_prune",&q3_fast_recon_fast_modulo,&rand_100_gen,20480,2,16,5120);
	// perf_q3_rows("early_prune",&q3_fast_recon_fast_modulo,&rand_100_gen,20480,2,16,5120);
	// perf_q3_rows("early_prune",&q3_fast_recon_fast_modulo,&rand_100_gen,20480,2,16,5120);

	// printf("%f\n",gt_out_size/((double)cart_prod(1024,1024)));
	// validate_query(&q3_with_prune,Q3);
	// validate_query(&q3_vector_v5
	// ,Q3);
	// test_integer_vector_division(10000);
	// test_integer_vector_mod(1000);
	
	// validate_query(&q2_vector_weave, Q2);
	// validate_query(&q3_weave_index_vertical_block_v3,Q3);
	// validate_query(&q2_weave,Q2);

	//test_fast_recon(256,2,asc_gen);
	// take_perf(28000,20);
	// test_integer_vector_mod(10000);
	// test_q3()
	// test_fast_recon(256,2,asc_gen);
	// fast_recon()
	// bool correct;

	// correct = test_q3(&q3_weave_index_l1_block,rand_gen,	mod_gen,1024,4,1024,4);	
	// // // correct = test_q3(&q3_vector_v5,rand_gen,mod_gen,256,4,128,4);

	// // correct = test_q3(&q3_weave_index_vertical_block_v2,rand_gen,one_zero_gen,256,4,128,4);
	// correct = test_q3(&q3_weave_index_vertical_block_v2,rand_gen,one_zero_gen,32768,4,32768,4);

	// // correct = test_q3(&q3_weave_index_vertical_block_v3,rand_gen,one_zero_gen,32768,4,32768,4);

	// printf("Correct: %d\n",correct);

	// perf_q3_rows("vert_block_v3,B=1",&q3_weave_index_vertical_block_v3,&one_zero_gen,32768,2,16,5120);



	// perf_q3_rows("l1_block_rand_gen,R_cols=32",&q3_weave_index_l1_block,&rand_gen,20480,32,16,5120);
	// perf_q3_rows("vector_v5_rand_gen,R_cols=32",&q3_vector_v5,&rand_gen,20480,32,16,5120);

	// fast_recon_perf();

	// perf_q3_rows("base_line",&q3_weave_index,&one_zero_gen,20480,32,4,5120);
	// perf_q3_rows("l1_block_rand_gen,one_zero,R=32",&q3_weave_index_l1_block,&one_zero_gen,20480,32,4,5120);
	// perf_q3_rows("vector_v5_rand_gen,one_zero,R=32",&q3_vector_v5,&one_zero_gen,20480,32,4,5120);
	// perf_q3_rows("vector_v5_rand_gen,one_zero,R=32",&q3_fast_recon_fast_modulo,&one_zero_gen,20480,32,4,5120);
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
	/*
	void (*queries[6])() = {&q1_weave, &q1_weave_v2, &q1_weave_v3, &q1_weave_v4, &q1_weave_v5, &q1_weave_v6};
	
	performance_rnd_query_v2(queries, Q1, "weaveOut.csv", 6);
	*/
	/*
	void (*queries[10])() = {&q1_parallel_weave, &q1_parallel_weave_v2, &q1_vector_weave, &q1_vector_weave_v2, q1_vector_weave_v3, q1_vector_weave_v3_1, q1_vector_weave_v4, q1_vector_weave_v4_1, q1_vector_weave_v5, q1_vector_weave_v6};
	
	performance_rnd_query_v2(queries, Q1, "vectorOut.csv", 10);
	*/
	//profile_q1(&q1_weave_v4, asc_gen, 512, 16);
	// profile_q1(&q1_parallel_weave_v3, asc_gen, 2048, 4);
	// profile_q1(&q1_parallel_weave_v3, asc_gen, 2048, 32);
	
	// // void (*queries[6])() = {&q1_weave, &q1_weave_v2, &q1_weave_v3, &q1_weave_v4, &q1_weave_v5, &q1_weave_v6};
	
	// // performance_rnd_query_v2(queries, Q1, "testout.csv", 6);
	
	// void (*queries[3])() = {&q1_parallel_weave, &q1_parallel_weave_v2, &q1_parallel_weave_v3};
	
	//performance_rnd_query_v2(queries, Q1, "parallel_weave_v3.csv", 3);
	// correct = test_q3(&q3_vector_v3,mod_gen,mod_gen,1024,2,1024,4);
	// printf("Correct: %d\n",correct);
	


	// perf_q3_R_rows("q3_weave_index_l1_block",&q3_weave_index_l1_block,&rand_gen,32768,1024 ,2,1024,4);
	// perf_q3_R_rows("q3_weave_index",&q3_weave_index,&rand_gen,32768,1024 ,2,1024,4);

	
	return 0;
}
