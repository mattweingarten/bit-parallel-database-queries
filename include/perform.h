#ifndef PERFORM
#define PERFORM 

#include <bits/stdint-uintn.h>
#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>  
#include "../include/query.h"
#include "../include/generator.h"
#include "../include/validate.h"


// typedef uint32_t (*generator) (size_t,size_t); 
#define N_PERFORMANCE_INTERATION 1000
#define N_WARMUP 1000
#define PRINT_CYCLES 1

void performance_rnd_query(void* query, enum Query type);

/// assumes set up data vars
//void performance_query(void* query, enum Query type);

//void setup_performance_db(uint32_t* data,size_t rows, size_t cols);

double perf_test_q1(q1_t q,generator gen,size_t rows,size_t cols);
double perf_test_q2(q2_t q,generator gen,size_t rows,size_t cols);
//double perf_test_q3(q3_t, generator R_gen,generator S_gen,size_t R_rows,size_t R_cols, size_t S_rows,size_t S_cols);

void saveCycledataToFile(char* filename,size_t cycles, size_t rows, size_t cols, size_t gen);

#endif 
