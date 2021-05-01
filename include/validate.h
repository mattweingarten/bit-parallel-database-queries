#ifndef VALIDATE
#define VALIDATE

#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>  


#define N_RUNS 10


typedef uint32_t (*generator) (size_t,size_t); 

typedef void (*q1_type) (uint32_t * data,uint32_t * dest,uint32_t *temps,int word_size,int block_size,int num_features, int num_samples,int number_entries);

typedef uint64_t (*q2_type) (uint32_t * data,uint32_t * cond_buffer,uint32_t *temp_buffer,uint32_t *sum_buffer,int word_size,int block_size,int num_features, int num_samples,int number_entries);



enum Query {
    Q1,
    Q2,
    Q3
};

typedef union query_t {
    q1_type q1;
    q2_type q2;
} query_t;


void validate_query(query_t query, enum Query type);
//generator in standard row based implementation
uint32_t* generateDB(size_t rows,size_t cols, generator);
// generators
uint32_t rand_gen(size_t i, size_t j );
uint32_t asc_gen(size_t i, size_t j);
uint32_t i_gen(size_t i, size_t j);
uint32_t j_gen(size_t i, size_t j);
uint32_t mod_gen(size_t i, size_t j);

uint32_t* q1_groundtruth(uint32_t* data,size_t rows,size_t cols);
uint64_t q2_groundtruth(uint32_t* data,size_t rows,size_t cols);

uint32_t *weave_samples_wrapper(uint32_t* data,size_t rows,size_t cols);
uint32_t *q1_wrapper(q1_type q,uint32_t* data,size_t rows,size_t cols);

uint64_t q2_wrapper(q2_type q,uint32_t* data,size_t rows,size_t cols);

bool test_q1(q1_type q,generator gen,size_t rows,size_t cols);
bool test_q2(q2_type q,generator gen,size_t rows,size_t cols);

bool compare(uint32_t * x, uint32_t *y,size_t n);

#endif VALIDATE