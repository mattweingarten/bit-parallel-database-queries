
#include "include/load.h"
#include "include/converter.h"
#include "include/debug.h"
#include "include/query1.h"
#include "include/query2.h"
#include "include/query3.h"
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

	query_t q1;
	q1.q1 = &q1_weave;
	query_t q2;
	q2.q2 = &q2_weave;

	validate_query(q1,Q1);
	validate_query(q2,Q2);

	converter_test(2560, 4, 1111);
	// test_q2(&q2_weave,&rand_gen,256,4);	
	return 0;
}