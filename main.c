
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


	printf("Size : %d\n", rows * cols );


	int num_blocks = (int) ceil(rows * cols / 512.0);

	uint32_t * src = (uint32_t* ) malloc(rows * cols * sizeof(uint32_t));
	uint32_t * dest = (uint32_t* ) malloc(num_blocks * 32 * 16 * sizeof(uint32_t));
	uint32_t* q1_dest = (uint32_t*) malloc(rows * sizeof(uint32_t));

	uint32_t count = 0;
	for(int i = 0; i < rows;++i){
		for(int j = 0; j < cols;++j){
			if(i <= rows){
				// if(j % 2 == 0){
					src[i * cols + j] = 1L << 31 ;
					src[i * cols + j] += 1L << 30;
				// }
				
			}
			
			// printf("src[%d] = %d \n",i*cols + j,);
			count++;
		}	
	}
	PRINT_MALLOC(src,rows,cols);
	PRINT_MALLOC_B(src,rows,cols);

	weave_samples_simple(dest,src,rows,cols);
	printf("PRINT WEAVED::\n\n");
	// PRINT_WEAVED(dest,rows,cols);
	PRINT_MALLOC_B(dest,10,4);
	// PRINT_MALLOC(dest,rows,cols);
	LINE;LINE;
	//PRINT_MALLOC_B(dest,rows,cols);


	//straightforward q1

	q1(src,q1_dest,rows,cols);
	printf("Q1 result:\n");
	PRINT_MALLOC(q1_dest,rows,1);
	printf("Q2 result:\n");
	printf("%d\n",q2(src,rows,cols));
	free(src);
	free(dest);
	free(q1_dest);

	// uint8_t y = 0b00000000;
	// uint8_t x = 0b00000001;
	// PRINT_B(x); LINE;
	// PRINT_B(y); LINE;
	// PRINT_B(y - x);

	test_lt();
	return 0;
}