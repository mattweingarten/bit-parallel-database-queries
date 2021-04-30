
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

	query_t query_1;
	query_1.q1 = &q1_weave;
	query_t query;
	query.q2 = &q2_weave;

	validate_query(query_1,Q1);
	converter_test(2560, 4, 1111);

	return 0;
}