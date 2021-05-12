
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






	// bool correct = test_q3(&q3_weave_index_l1_block,asc_gen,asc_gen,512,2,128,4);

	// printf("Correct = %d\n",correct);

	validate_query(q3_weave_index_l1_block,Q3);

	// double res = perf_test_q3(&q3_weave_index,rand_gen,rand_gen,512,4,128,8);
	// printf("Res: %lf\n",res);
	// perf_q3_R_rows("q3_test", q3_weave_index,&rand_gen,1024 * 100,1024,2,1024,4); 


	// perf_q3_R_rows("q3_standard_measurement",&q3_weave_index,&rand_gen,64 * 1024,1024 ,2,128,4);

	// validate_query((void*) &q1_weave,Q1);
	// validate_query((void*) &q1_vector_weave,Q1);
	// validate_query((void*) &q1_parallel_weave,Q1);
	// validate_query((void*) &q2_weave, Q2);
	// validate_query((void* ) &q3_weave_index,Q3);

	
	//performance_rnd_query((void*) &q1_parallel_weave, Q1);
	
	// performance_rnd_query((void*) &q2_weave, Q2);
	return 0;
}
