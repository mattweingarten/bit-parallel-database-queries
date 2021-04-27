
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query1.h"
#include <math.h>
//Select * FROM R WHERE R.a < R.b

//Straightforward
//TODO
void q1(uint32_t * data,uint32_t * dest,int rows,int cols){
    for(int i = 0;i < rows;++i){
        if(data[i * cols + 0] < data[i * cols + 1]){
            dest[i] = 1;
        }else{
            dest[i] = 0;
        }
    }
}


void q1_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_features, int num_samples,int number_entries){
    int chunk_index;
	int feature_index;
	uint32_t a;
	uint32_t b;
	uint32_t xor;

    int rows_per_word = word_size / num_features;
	int samples_per_block = block_size / num_features;
    int num_blocks = ceil(number_entries / block_size);
    for(int k = 0; k < num_blocks;k++){
        for(int j = 0; j < 32;++j){ // rows per block
		// index = 0;
            for(int i = 0; i < samples_per_block; ++i){// blocksize / 4
                chunk_index = i / rows_per_word; //rows per word
                feature_index = i % rows_per_word; // rows per word
                a = data[k * block_size + 16 * j  + chunk_index] >> (feature_index * num_features) &1;
                b = data[k * block_size + 16 * j  + chunk_index] >> (feature_index * num_features + 1) &1; // 16 for number of cols in block
                xor =  a ^ b;
                results[k * block_size + i] = results[k * block_size + i] | ((xor & b) & (!temps[i]));
                temps[k * block_size + i] = temps[k * block_size + i] | (xor & a);
            } 
	    }
    }
}