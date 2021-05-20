#ifndef QUERY3
#define QUERY3

#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct modular_operation {
    uint32_t m;
    uint32_t sh1;
    uint32_t sh2;
};


uint32_t cartesian_product_size(int R_rows, int R_cols, int S_rows, int S_cols);


void q3(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * output_size, int R_rows, int R_cols, int S_rows, int S_cols);
void q3_index(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols);



void q3_weave(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows,uint32_t *R_buffer, uint32_t *S_buffer, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);

void q3_weave_index(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows,size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);


void q3_weave_index_l1_block(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);
void q3_prune(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);

void q3_unroll(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);

void q3_vector(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);


void q3_weave_index_l2_block(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size,size_t block_size);


void q3_unroll_v2(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);

void q3_vector_v2(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);
void q3_vector_v3(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);

void q3_vector_v4(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size);
#endif
