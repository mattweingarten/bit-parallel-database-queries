
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query2.h"

//Select SUM(c) FROM R WHERE R.a < R.b

//Straightforward
//TODO

uint64_t q2(uint32_t * data,int rows,int cols){
    uint64_t sum = 0;
    for(int i = 0;i < rows;++i){
        // printf("[%d],res=%u,val=%u\n",i,data[i * cols + 0] < data[i * cols + 1],data[i * cols + 2]);
        if(data[i * cols + 0] < data[i * cols + 1]){
            sum += data[i * cols + 2];
        }
        
    }
    
    return sum;
}

uint64_t q2_weave(uint32_t * data,uint32_t * cond_buffer,uint32_t *temp_buffer,uint32_t *sum_buffer,int word_size,int block_size,int num_features, int num_samples,int number_entries){

    // printf("Got here!\n");
    int chunk_index;
	int feature_index;
	uint32_t a;
	uint32_t b;
    uint32_t c;
	uint32_t xor;
    uint64_t sum;

    int samples_per_word = word_size / num_features;
	int samples_per_block = block_size / num_features;
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
    int cols_per_block = word_size;
    for(int k = 0; k < num_blocks;k++){
        for(int j = 0; j < word_size;++j){ 
            for(int i = 0; i < samples_per_block; ++i){ 
                chunk_index = i / samples_per_word; 
                feature_index = i % samples_per_word; 
                a = data[k * block_size + rows_per_block * j  + chunk_index] >> (feature_index * num_features) &1;
                b = data[k * block_size + rows_per_block * j  + chunk_index] >> (feature_index * num_features + 1) &1;
                c = data[k * block_size + rows_per_block * j  + chunk_index] >> (feature_index * num_features + 2) &1;
                xor =  a ^ b;
                cond_buffer[i] = cond_buffer[i] | ((xor & b) & (!temp_buffer[i]));
                temp_buffer[i] = temp_buffer[i] | (xor & a);
                sum_buffer[i] =  sum_buffer[i] + (c << (31 - j));
                // printf("%d\n",(31 - j));   
            }

	    }

        // TODO: this part should be easily vectorizable and we can multiple accumalators!
        for(int i = 0; i < samples_per_block;++i){
            // printf("[%d]:, res_buffer=[%u],sumbuffer=%u,\n",i,cond_buffer[i],sum_buffer[i]);

            if(cond_buffer[i]){ // use mm_256_blendvd for branch elimination
                sum += sum_buffer[i]; // use acc for sum
            }
        }

        memset(sum_buffer,0,4 * samples_per_block);
        memset(temp_buffer,0,4 * samples_per_block);
        memset(cond_buffer,0,4 * samples_per_block);
        // PRINT_MALLOC(sum_buffer,samples_per_block,1); 
        //
        
    }
    return sum;
}