
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




	// test_q3(&q3_unroll,mod_gen,mod_gen,128,4,128,4);
	// test_q3(&q3_unroll_v2,mod_gen,mod_gen,128,4,128,4);
	bool correct = test_q3(&q3_vector,mod_gen,mod_gen,256,2,128,4);
	printf("%d\n",correct);
	// validate_query(&q3_vector,Q3);
	// validate_query(&q3_weave_index_l2_block,Q3b);
	// perf_q3_compare_block("q3_blocking_measurements",&q3_weave_index_l2_block,16384);


	// perf_q3_R_rows("q3_weave_index_l1_block",&q3_weave_index_l1_block,&rand_gen,32768,1024 ,2,1024,4);
	// perf_q3_R_rows("q3_weave_index",&q3_weave_index,&rand_gen,32768,1024 ,2,1024,4);

	
	return 0;
}
