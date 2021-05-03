
#include "include/load.h"
#include "include/converter.h"
#include "include/debug.h"
#include "include/query1.h"
#include "include/query2.h"
#include "include/query3.h"
#include "include/query.h"
#include "include/unit_tests.h"
#include "include/validate.h"
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>  
#include <assert.h>


int main(int argc, char **argv) {

	uint32_t* R =  generateDB(256,2,asc_gen);
	uint32_t* S =  generateDB(128,4,mod_gen);
	uint32_t out_size = cartesian_product_size(256,2,128,4);


	// uint32_t* results = malloc(256 * sizeof(uint32_t));
    // uint32_t *temps = malloc(256 * sizeof(uint32_t));
	// size_t numEntries = numberOfEntries(256,12);

	// q1_weave(R,results,temps,32,512,256,2,numEntries);


	// validate_query((void*) q2_weave,Q2);
	// validate_query((void*) q1_parallel_weave,Q1);

	// printf("Output size: %u\n",out_size);


	// PRINT_MALLOC(R,256,2);
	// PRINT_MALLOC(S,128,4);


	// int x = 1 % 0;
	
	// q3(R,S,dest,&out_size,256,2,128,4);
	// printf("Output size: %u\n",out_size);
	// realloc(dest,out_size * 6 *  sizeof(uint32_t));
	// PRINT_MALLOC(dest,out_size,6);

	// q3_weave()
	// union query_t q;
	// q.q1 = q1_weave;
	//query_t q;
	//q.q1 = q1_weave;
 	//validate_query( (void*)&q1_weave,Q1);
	//validate_query((query_t) q,Q1);


	bool c = test_q1(q1_vector_weave,rand_gen,256,8);
	printf("Correct : %i \n", c);
	//validate_query(&q1_vector_weave, Q1);
	// converter_test(2560, 4, 1111);
	// test_q2(&q2_weave,&rand_gen,256,4);

	// uint32_t * R_weave = weave_samples_wrapper(R,256,2);
	// uint32_t * S_weave = weave_samples_wrapper(S,128,4);
	// // // // PRINT_WEAVED(R_weave,256,2);

	// uint32_t* R_buffer = (uint32_t*) malloc(2 * sizeof(uint32_t));
	// uint32_t* S_buffer = (uint32_t*) malloc(4 * sizeof(uint32_t));
	// uint32_t* dest = (uint32_t*) malloc(out_size * sizeof(uint32_t));
	// q3_weave(R_weave,S_weave,dest,&out_size,R_buffer,S_buffer,256,2,128,4,32,16);
	// realloc(dest,out_size * 6 *  sizeof(uint32_t));
	// // PRINT_MALLOC(dest,out_size, 6);

	// uint32_t out_size_1 = cartesian_product_size(256,2,128,4);
	// uint32_t* dest_1 = (uint32_t*) malloc(out_size_1 * sizeof(uint32_t));
	// q3(R,S,dest_1,&out_size_1,256,2,128,4);
	// realloc(dest_1,out_size_1 * 6 *  sizeof(uint32_t));
	// // PRINT_MALLOC(dest_1,out_size_1, 6);

	// bool correct = test_q3(&q3_weave,&asc_gen,&asc_gen,256,4,256,4);
	// printf("Result of test: %d\n",correct);
	validate_query((void*) &q1_weave,Q1);
	validate_query((void*) &q1_vector_weave,Q1);
	validate_query((void*) &q1_parallel_weave,Q1);
	// validate_query((void*) &q2_weave, Q2);
	// validate_query((void* ) &q3_weave,Q3);

	// printf("Outsize q3=%d,outsize q3_weave=%d\n",out_size,out_size_1);
	// printf("Q3 weave == q3: %d\n",compare_rows_cols(dest_1,dest,out_size,6));
	return 0;
}