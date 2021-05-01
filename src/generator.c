#include "../include/generator.h"

#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include<stdbool.h>  
#include <stdarg.h>


//generators to populate malloc
uint32_t rand_gen(size_t i, size_t j ){
    	return ((uint32_t) rand() << 1) + ((uint32_t) rand() % 2);
}

uint32_t asc_gen(size_t i, size_t j){
    return i + j;
}

uint32_t i_gen(size_t i, size_t j){
    return i;
}


uint32_t j_gen(size_t i, size_t j){
    return j;
}

uint32_t mod_gen(size_t i, size_t j){
    return i + j % 10;
}

uint32_t one_gen(size_t i, size_t j){
    return 1;
}

uint32_t one_zero_gen(size_t i, size_t j){
    return (j ) % 2;
}

uint32_t two_zero_gen(size_t i, size_t j){
    if((j + 1) %2 == 0){
        return 2;
    }
    else {
        return 0;
    }
}


uint32_t* generateDB(size_t rows,size_t cols, generator gen){
    uint32_t* res = (uint32_t*)  malloc(rows * cols * sizeof(uint32_t));
    for(size_t i = 0; i < rows;++i){
		for(size_t j = 0; j < cols;++j){
            res[i * cols + j] = gen(i,j);
        }	
	}
    return res;
}