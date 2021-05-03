#include <stdint.h>

#include "../include/debug.h"
void PRINT_MALLOC(uint32_t* ptr, size_t rows, size_t cols){
    for(int i = 0; i < rows;++i){
		for(int j = 0; j < cols;++j){
			printf("%u ",ptr[i * cols + j]);
			// printf("[%d,%d]:%u ",i,j,ptr[i * cols + j]);
		}
		LINE;
	}
}

void PRINT_MALLOC_B(uint32_t* ptr,size_t rows, size_t cols){
    for(int i = 0; i < rows;++i){
		for(int j = 0; j < cols;++j){
            PRINT_32_B(ptr[i * cols + j]);
            BORDER;
		}
		LINE;
	}
}

void PRINT_MALLOC_H(uint32_t* ptr,size_t n){
    for(int i = 0; i < n;++i){
		printf(" %u ",ptr[i]);
	}
}

void PRINT_WEAVED(uint32_t* ptr,size_t rows, size_t cols){
	int num_blocks = ceil(rows * cols / 512);
	printf("Number of blocks:%d\n",num_blocks);
	// per block, we have 512 entries (i index)
	// when woven, we have 16 32bit values per row
	// the lowest bits of the first value == the highest bits of first sample / first feature
	// to fetch the next bit of one woven value we have to go through an entire row (16 values) -> j index
	// as of now, with k = 3, we show the bottom 4 * 32 (so 128) values in their entire 32 bits
    for(int i = 0; i < num_blocks;++i){
		HLINE;
		for(int j = 0; j < 32;++j){ // should be n_bits (size of data representation)
		// Shows the first 32 * 4 entries
			for(int k = 0; k <= 15; k++){
				if(k % 4 == 0){
					LINE;
				}
				PRINT_32_B(ptr[(i * 512) + (16 * j) + k]);
				BORDER;
			}
			LINE;
			LINE;
			HBORDER;
		LINE;
		}
		HLINE;
	}
}
