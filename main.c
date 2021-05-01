
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



 	validate_query((query_t) &q2_weave,Q2);
	validate_query((query_t) &q1_weave,Q1);

	
	converter_test(2560, 4, 1111);
	// test_q2(&q2_weave,&rand_gen,256,4);
	
	return 0;
}