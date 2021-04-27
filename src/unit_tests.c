#include<stdbool.h>  
#include <assert.h>
#include <stdint.h> 
#include "../include/unit_tests.h"

uint32_t lt(uint32_t x, uint32_t y){
	uint32_t res = 0;
	uint32_t temp = 0;
	for(int i = 31; i >=0;i--){
		uint32_t xi = (x >> i) &1;
		uint32_t yi = (y >> i) &1;
		uint32_t xor = xi ^ yi;
		res = res | ((xor & yi) & (!temp));
		temp = temp | (xor & xi);
	}
	return res;
}

void test_lt(void){
    for(uint32_t i = 0;i < 1L << 23;i++){
		uint32_t x = (uint32_t) rand();
		uint32_t y = (uint32_t) rand();
		assert(lt(x,y) == x < y && "LT test did not pass!");
	}
    printf("Test lt passed!\n");
}

bool compare(uint32_t * x, uint32_t *y,int N){
    bool res = true;
    for(int i = 0; i < N; ++i){
        // printf("[%d]=>%d,%d\n",i,x[i],y[i]);
        res = res && (x[i] == y[i]);
    }
    return res;
}