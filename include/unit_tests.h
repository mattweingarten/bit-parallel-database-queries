#ifndef UNIT
#define UNIT

#include <stdint.h> 
#include<stdbool.h>  


uint32_t lt(uint32_t x, uint32_t y);
void test_lt(void);
void test_integer_vector_division(size_t N);
void fast_integer_division(uint32_t * x,uint32_t y,  uint32_t * dest,size_t N);
uint32_t fast_uint_div(uint32_t dividend, uint32_t d);
uint32_t fast_uint_div_v2(uint32_t dividend, uint32_t d);
void test_integer_vector_mod(size_t N);
void fast_integer_mod(uint32_t * x,uint32_t d,  uint32_t * dest,size_t N);

#endif UNIT