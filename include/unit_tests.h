#ifndef UNIT
#define UNIT

#include <stdint.h> 
#include<stdbool.h>  
#include "generator.h"

uint32_t lt(uint32_t x, uint32_t y);
void test_lt(void);
void test_integer_vector_division(size_t N);
void fast_integer_division(uint32_t * x,uint32_t y,  uint32_t * dest,size_t N);
uint32_t fast_uint_div(uint32_t dividend, uint32_t d);
uint32_t fast_uint_div_v2(uint32_t dividend, uint32_t d);
void test_integer_vector_mod(size_t N);
void fast_integer_mod(uint32_t * x,uint32_t d,  uint32_t * dest,size_t N);
void fast_integer_mod2(uint32_t * x,uint32_t d,  uint32_t * dest,size_t N);
void test_fast_recon(size_t rows,size_t cols, generator gen);
void fast_recon(uint32_t * src, uint32_t *dest,size_t rows, size_t cols);
void fast_recon_v2(uint32_t * src, uint32_t *dest,size_t rows, size_t cols);
void fast_recon_v3(uint32_t * src, uint32_t *dest,size_t rows, size_t cols);
void perf_mod(size_t N);
void perf_mod_2(size_t N);
void perf_mod_3(size_t N);
void take_perf(size_t maxN,size_t steps);
void base_line_integer_mod(uint32_t * x,uint32_t d,  uint32_t * dest,size_t N);
void fast_recon_perf(void);
#endif UNIT