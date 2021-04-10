
#include "include/load.h"
#include "include/converter.h"
#include "include/debug.h"
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
	int rows = 12; //numsamples
	int cols = 2;  // numfeatures


	printf("%d\n", rows * cols * sizeof(uint32_t));

	uint32_t * src = (uint32_t* ) malloc(rows * cols * sizeof(uint32_t));
	printf("got here\n");

	uint32_t count = 0;
	for(int i = 0; i < rows;++i){
		for(int j = 0; j < cols;++j){
			src[i * rows + j] = count;
			count++;
		}	
	}
	PRINT_MALLOC(src,rows,cols);
	PRINT_MALLOC_B(src,rows,cols);
	uint32_t * dest = (uint32_t* ) malloc(rows * cols * sizeof(uint32_t));
	weave_samples_simple(dest,src,rows,cols);
	PRINT_MALLOC(dest,rows,cols);
	PRINT_MALLOC_B(dest,rows,cols);
	free(src);
	free(dest);
	return 0;
}