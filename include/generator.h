#ifndef GENERATE
#define GENERATE
#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h> 


typedef uint32_t (*generator) (size_t,size_t); 


uint32_t* generateDB(size_t rows,size_t cols, generator);
// generators
uint32_t rand_gen(size_t i, size_t j );
uint32_t asc_gen(size_t i, size_t j);
uint32_t i_gen(size_t i, size_t j);
uint32_t j_gen(size_t i, size_t j);
uint32_t mod_gen(size_t i, size_t j);


#endif 