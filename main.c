
#include "include/load.h"
#include "include/converter.h"
#include "include/debug.h"
#include "include/query1.h"
#include "include/query2.h"
#include "include/query3.h"

#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



int main(int argc, char **argv) {


	// char * file = argv[1];
	// printf("lines: %d\n",count_rows(file));
	// printf("columns: %d\n",count_columns(file));
	// get_table(file,count_rows(file),count_columns(file));
	//
	int rows = 8; //numsamples
	int cols = 3;  // numfeatures?


	printf("Size : %d\n", rows * cols );

	uint32_t * src = (uint32_t* ) malloc(rows * cols * sizeof(uint32_t));

	uint32_t count = 0;
	for(int i = 0; i < rows;++i){
		for(int j = 0; j < cols;++j){
			src[i * cols + j] = count %5;
			printf("src[%d] = %d \n",i*cols + j,count);
			count++;
		}	
	}
	PRINT_MALLOC(src,rows,cols);
	PRINT_MALLOC_B(src,rows,cols);
	uint32_t * dest = (uint32_t* ) malloc(rows * cols * sizeof(uint32_t));
	weave_samples_simple(dest,src,rows,cols);
	PRINT_MALLOC(dest,rows,cols);
	LINE;LINE;
	PRINT_MALLOC_B(dest,rows,cols);


	//straightforward q1
	uint32_t* q1_dest = (uint32_t*) malloc(rows * sizeof(uint32_t));
	q1(src,q1_dest,rows,cols);
	uint64_t sum = q2(src,rows,cols);

	printf("Q1 result:\n");
	PRINT_MALLOC(q1_dest,rows,1);
	printf("Q2 result:\n");
	printf("%d\n",sum);
	
	free(src);
	free(dest);
	return 0;
}