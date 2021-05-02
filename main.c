
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
#include<stdbool.h>  
#include <assert.h>


int main(int argc, char **argv) {


	uint32_t* R =  generateDB(256,2,asc_gen);
	uint32_t* S =  generateDB(128,4,mod_gen);
	uint32_t out_size = cartesian_product_size(256,2,128,4);
	printf("Output size: %u\n",out_size);


	PRINT_MALLOC(R,256,2);
	PRINT_MALLOC(S,128,4);


	// int x = 1 % 0;
	uint32_t* dest = (uint32_t*) malloc(out_size * sizeof(uint32_t));
	
	q3(R,S,dest,&out_size,256,2,128,4);
	printf("Output size: %u\n",out_size);
	realloc(dest,out_size * 6 *  sizeof(uint32_t));
	PRINT_MALLOC(dest,out_size,6);

 	// validate_query((query_t) &q2_weave,Q2);
	// validate_query((query_t) &q1_weave,Q1);

	
	// converter_test(2560, 4, 1111);
	// test_q2(&q2_weave,&rand_gen,256,4);
	
	return 0;
}