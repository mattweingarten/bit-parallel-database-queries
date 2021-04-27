
#include "include/load.h"
#include "include/converter.h"
#include "include/debug.h"
#include "include/query1.h"
#include "include/query2.h"
#include "include/query3.h"
#include "include/unit_tests.h"

#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include<stdbool.h>  
#include <assert.h>


int main(int argc, char **argv) {
	int rows = 128; //numsamples
	int cols = 4;  // numfeatures?
	int numEntries = numberOfEntries(rows,cols);

	printf("Size : %d\n", rows * cols );
	printf("Numentries : %d\n", numEntries );


	uint32_t * src = (uint32_t* ) malloc(rows * cols * sizeof(uint32_t));
	uint32_t * dest = (uint32_t* ) malloc(numEntries * sizeof(uint32_t));
	uint32_t* q1_dest = (uint32_t*) malloc(rows * sizeof(uint32_t));

	uint32_t count = 0;
	for(int i = 0; i < rows;++i){
		for(int j = 0; j < cols;++j){
			// if(i <= 1){
				// if(j == 1){
					// src[i * cols + j] = (uint32_t) 1L << 31;
				// }
					
					src[i * cols + j] = (uint32_t) (rand());				
			// }			
			count++;
		}	
	}
	PRINT_MALLOC(src,rows,cols);
	PRINT_MALLOC_B(src,rows,cols);
	weave_samples(dest,src,rows,cols);
	LINE;LINE;
	PRINT_WEAVED(dest,rows,cols);
	LINE;LINE;

	numEntries = numberOfEntries(rows,cols);
	uint32_t* results = malloc(rows * sizeof(uint32_t));
	uint32_t * temps = malloc(rows * sizeof(uint32_t));
	memset(results,0,rows*4);
	memset(temps,0,rows*4);

	q1_weave(dest,results,temps,32,512,cols,rows,numEntries);

	

	// printf("Num entries total :%d\n",numEntries);
	// assert(count == numEntries);

	//straightforward q1

	// PRINT_MALLOC(results,128,1);
	LINE;
	q1(src,q1_dest,rows,cols);
	// printf("Q1 result:\n");
	// PRINT_MALLOC(q1_dest,rows,1);
	assert(compare(results,q1_dest,rows));
	// printf("Q2 result:\n");
	// printf("%d\n",q2(src,rows,cols));
	free(src);
	free(dest);
	free(q1_dest);
	converter_test(2560, 4, 1111);
	test_lt();
	return 0;
}