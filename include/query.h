#ifndef QUERY
#define QUERY

#include "../include/query1.h"
#include "../include/query2.h"
#include "../include/query3.h"



typedef void (*q1_t) (uint32_t * data,uint32_t * dest,uint32_t *temps,int word_size,int block_size,int num_features, int num_samples,int number_entries);

typedef uint64_t (*q2_t) (uint32_t * data,uint32_t * cond_buffer,uint32_t *temp_buffer,uint32_t *sum_buffer,int word_size,int block_size,int num_features, int num_samples,int number_entries);



enum Query {
    Q1,
    Q2,
    Q3
};

typedef union query_t {
    q1_t q1;
    q2_t q2;
} query_t;







#endif 