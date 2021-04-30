
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

    int samples_per_word = word_size / num_features;
	int samples_per_block = block_size / num_features;
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
    int cols_per_block = word_size;
    printf("Staring query 1 with:num_blocks=%d,block_size=%d, samples_per_block = %d, samples_per_word = %d,num_features= %d\n",num_blocks,block_size,samples_per_block,samples_per_word);
    for(int k = 0; k < num_blocks;k++){
        for(int j = 0; j < word_size;++j){ 
            for(int i = 0; i < samples_per_block; ++i){ 
                chunk_index = i / samples_per_word; 
                feature_index = i % samples_per_word; 
                a = data[k * block_size + rows_per_block * j  + chunk_index] >> (feature_index * num_features) &1;
                b = data[k * block_size + rows_per_block * j  + chunk_index] >> (feature_index * num_features + 1) &1;
                xor =  a ^ b;
                results[k * samples_per_block + i] = results[k * samples_per_block + i] | ((xor & b) & (!temps[k * samples_per_block + i]));
                temps[k * samples_per_block + i] = temps[k * samples_per_block + i] | (xor & a);
            } 
	    }
    }
}