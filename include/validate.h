#ifndef VALIDATE
#define VALIDATE

#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>  
#include "../include/query.h"
#include "../include/generator.h"

#define DEBUG 0
#define N_RUNS 1


// typedef uint32_t (*generator) (size_t,size_t); 


// void single_test(union query_t query, enum Query type);
void validate_query(void* query, enum Query type);
//generator in standard row based implementation

uint32_t* q1_groundtruth(uint32_t* data,size_t rows,size_t cols);
uint64_t q2_groundtruth(uint32_t* data,size_t rows,size_t cols);

uint32_t *q1_wrapper(q1_t q,uint32_t* data,size_t rows,size_t cols);

uint64_t q2_wrapper(q2_t q,uint32_t* data,size_t rows,size_t cols);

bool test_q1(q1_t q,generator gen,size_t rows,size_t cols);
bool test_q2(q2_t q,generator gen,size_t rows,size_t cols);
bool test_q3(q3_t, generator R_gen,generator S_gen,size_t R_rows,size_t R_cols, size_t S_rows,size_t S_cols);

bool compare(uint32_t * x, uint32_t *y,size_t n);
bool compare_rows_cols(uint32_t * x, uint32_t *y,size_t rows,size_t cols);
#endif 