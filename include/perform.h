#ifndef PERFORM
#define PERFORM 

//#include <bits/stdint-uintn.h>
#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>  
#include "../include/query.h"
#include "../include/generator.h"
#include "../include/validate.h"


// typedef uint32_t (*generator) (size_t,size_t); 
#define N_PERF_ITERATION 10
#define N_WARMUP 1
#define PRINT_CYCLES 1

#define Q1_PATH "./runtimedata/q1/"
#define Q2_PATH "./runtimedata/q2/"
#define Q3_PATH "./runtimedata/q3/"
void performance_rnd_query(void* query, enum Query type,char * out_file_name);
void performance_rnd_query_v2(void** queries, enum Query type,char * out_file_name, int n_q_ver);
double profile_q1(q1_t q,generator gen,size_t rows,size_t cols);

/// assumes set up data vars
//void performance_query(void* query, enum Query type);

//void setup_performance_db(uint32_t* data,size_t rows, size_t cols);

double perf_test_q1(q1_t q,generator gen,size_t rows,size_t cols);
double perf_test_q2(q2_t q,generator gen,size_t rows,size_t cols);
double perf_test_q3(q3_t q,generator R_gen,generator S_gen,size_t R_rows,size_t R_cols, size_t S_rows,size_t S_cols);

void perf_q3_R_rows(char* filename, q3_t q,generator gen,size_t R_row_max, size_t step_size,size_t R_cols,size_t S_rows, size_t S_cols);
void perf_q3_rows(char* filename, q3_t q,generator gen,size_t row_max, size_t R_cols,size_t S_cols,size_t step_size);
double perf_test_q3_blocked(q3b_t q,generator R_gen,generator S_gen,size_t R_rows,size_t R_cols, size_t S_rows,size_t S_cols,size_t block_size);

void saveHeaderToFile( char* filename, size_t cols, size_t n_q_ver);
void saveCycledataToFile_v2( char* filename,size_t cycles, size_t rows, size_t cols, size_t flag);

void perf_q3_compare_block(char * filename, q3b_t q,size_t max_row_size);



void saveCycledataToFile(char* filename,size_t cycles, size_t rows, size_t cols, size_t gen);
void saveCycledataToFile_q3( char* filename,double cycles, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols);
void saveCycledataToFile_v1( char* filename,double cycles, size_t rows, size_t R_cols, size_t S_cols);
void saveCycledataToFile_q3_block_cmp( char* filename,size_t cycles, size_t R_rows, size_t S_rows,size_t block_size);
#endif 
