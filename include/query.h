#ifndef QUERY
#define QUERY

#include "../include/query1.h"
#include "../include/query2.h"
#include "../include/query3.h"



typedef void (*q1_t) (uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries);

typedef uint64_t (*q2_t) (uint32_t * data,uint32_t * cond_buffer,uint32_t *temp_buffer,uint32_t *sum_buffer,int word_size,int block_size,int num_samples,int num_features, int number_entries);

typedef void (*q3_t) (uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);
typedef void (*q3b_t) (uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size,size_t block_size);



// typedef void (*q3_t) (uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows,uint32_t *R_buffer, uint32_t *S_buffer, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);

enum Query {
    Q1,
    Q2,
    Q3,
    Q3b,
};





// typedef union query_t {
//     q1_t q1;
//     q2_t q2;
//     q3_t q3;
// } query_t;







#endif 