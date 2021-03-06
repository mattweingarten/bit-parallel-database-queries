#ifndef GENERATE
#define GENERATE
#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h> 


typedef uint32_t (*generator) (size_t,size_t); 


uint32_t* generateDB(size_t rows,size_t cols, generator);
void generate_selective_db(size_t rows,size_t cols, double alpha,uint32_t** S, uint32_t** R);
// generators
uint32_t rand_gen(size_t i, size_t j );
uint32_t rand_100_gen(size_t i, size_t j );
uint32_t asc_gen(size_t i, size_t j);
uint32_t i_gen(size_t i, size_t j);
uint32_t j_gen(size_t i, size_t j);
uint32_t mod_gen(size_t i, size_t j);
uint32_t one_gen(size_t i, size_t j);
uint32_t one_zero_gen(size_t i, size_t j);
uint32_t two_zero_gen(size_t i, size_t j);
uint32_t cartesian_product_size(int R_rows, int R_cols, int S_rows, int S_cols);
uint32_t cart_prod(size_t R_rows, size_t S_rows);
uint32_t rand_10_gen(size_t i, size_t j );
uint32_t first_col_gen(size_t i, size_t j);
uint32_t rand_1000_gen(size_t i, size_t j );
uint32_t j_bigger_by_up_to_5_bits(size_t i, size_t j);
uint32_t big_value_gen(size_t i, size_t j);
uint32_t top_16_bits_zero_gen(size_t i, size_t j);
uint32_t all_zero_gen(size_t i, size_t j);
#endif 