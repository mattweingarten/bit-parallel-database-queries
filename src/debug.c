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