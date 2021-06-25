#include "../include/generator.h"

#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include<stdbool.h>  
#include <stdarg.h>
#include <assert.h>


//generators to populate malloc
uint32_t rand_gen(size_t i, size_t j ){
        // time_t t;
        // srand((unsigned) time(&t));

    	return ((uint32_t) rand() << 1) + ((uint32_t) rand() % 2);
}

uint32_t rand_1000_gen(size_t i, size_t j ){
        // time_t t;
        // srand((unsigned) time(&t));
    	return ((uint32_t) rand() %1000);
}

uint32_t rand_100_gen(size_t i, size_t j ){
        // time_t t;
        // srand((unsigned) time(&t));
    	return ((uint32_t) rand() %100);
}
uint32_t rand_10_gen(size_t i, size_t j ){
        // time_t t;
        // srand((unsigned) time(&t));
    	return ((uint32_t) rand() %10);
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
    return (i + j) % 10;
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

uint32_t first_col_gen(size_t i, size_t j){
    if(j == 0){
        return i;
    }
    else {
        return 2;
    }
}

uint32_t all_zero_gen(size_t i, size_t j){
	return 0;
}

uint32_t big_value_gen(size_t i, size_t j){
	if(j == 0)
		return 4294967295;
	if(j == 1)
		return 4294967295 - 1048576;
	return rand();
}


uint32_t top_16_bits_zero_gen(size_t i, size_t j){
	return ((uint32_t) rand() % 1024);
}

uint32_t j_bigger_by_up_to_5_bits(size_t i, size_t j){
	if(j == 0)
		return 256;
	if(j == 1)
		return 256 << (i % 5);
	return rand() % 8192;
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


void generate_selective_db(size_t rows,size_t cols, double alpha,uint32_t** S, uint32_t** R){   


    uint32_t* res_R = (uint32_t*)  malloc(rows * cols * sizeof(uint32_t));
    uint32_t* res_S = (uint32_t*)  malloc(rows * cols * sizeof(uint32_t));
    
    assert(cols >= 4);

    for(size_t i = 0; i < rows;++i){
		for(size_t j = 0; j < cols;++j){
            res_R[i * cols + j] = ((uint32_t) rand() %1000);
        }
        res_S[i * cols] = ((uint32_t) rand() << 1) + ((uint32_t) rand() % 2);
        double r = (double)rand() / (double)RAND_MAX ;
        // printf("%f\n",r);
        if(r < alpha){
            res_S[i * cols + 1] = 1;
            res_S[i * cols + 2 ] = 0;
        }else{
            res_S[i * cols + 1] = ((uint32_t) rand() << 1) + ((uint32_t) rand() % 2);
            res_S[i * cols + 2 ] = ((uint32_t) rand() %1000) + 10000;
        }

        for(size_t j = 3; j < cols;++j){
            res_S[i * cols + j] =  ((uint32_t) rand() << 1) + ((uint32_t) rand() % 2);
        }
        
	}
    // printf("Here!%lx,%lx\n",S,R);
    *S = res_S;
    *R = res_R;
    // return res;
}


uint32_t cartesian_product_size(int R_rows, int R_cols, int S_rows, int S_cols){
	return R_rows * S_rows * (R_cols + S_cols);
}

uint32_t cart_prod(size_t R_rows, size_t S_rows){
    return R_rows * S_rows;
}