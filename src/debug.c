#include <stdint.h>

#include "../include/debug.h"
void PRINT_MALLOC(uint32_t* ptr, size_t rows, size_t cols){
    for(int i = 0; i < rows;++i){
		for(int j = 0; j < cols;++j){
			printf("%d ",ptr[i * cols + j]);
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

void PRINT_WEAVED(uint32_t* ptr,size_t rows, size_t cols){
	int num_blocks = rows * cols / 512;
	// per block, we have 512 entries
	// when woven, we have 16 32bit values per row
	// the lowest bits of the first value == the highest bits of first sample / first feature
	// to fetch every bit we have to go through an entire row (16 values)
	// as of now, with k = 3, we show the bottom four values in their entire 32 bits
    for(int i = 0; i < num_blocks + 1;++i){
		for(int j = 0; j < 32;++j){ // should be n_bits (size of data representation)
		// Shows the first 32 * 4 entries
			for(int k = 3; k >= 0; k--){
				PRINT_32_B(ptr[(i * 512) + (16 * j) + k]);
				BORDER;
			}
		LINE;
		}
	}
}