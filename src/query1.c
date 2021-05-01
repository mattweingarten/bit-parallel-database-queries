
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
    // printf("Staring query 1 with:num_blocks=%d,block_size=%d, samples_per_block = %d, samples_per_word = %d,num_features= %d\n",num_blocks,block_size,samples_per_block,samples_per_word);
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

void q1_parallel_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_features, int num_samples,int number_entries){
	
	// b == a >> 1
	uint64_t a;
	uint64_t b;
	
	uint64_t xor;
	
	
	int samples_per_block = 64 / num_features;
	int samples_per_cl = samples_per_block * 8;
   	int num_cl = ceil(((float)num_samples) / samples_per_cl);

	//printf("spb: %i, spcl: %i, num_cl: %i, ne: %i \n\n", samples_per_block, samples_per_cl, num_cl, number_entries);
	// write results to memory after every cacheline block 
	uint64_t temp[8] = {0}; //start as all 0's
	uint64_t res[8] = {0};
	
	// use 64bit pointer (do we need to cast?)
	uint64_t * d = data;
	
	/* i is the cacheline block index
	   j is the bit index (i.e. first to 32nd bit of each value)
	   k is the block index (we compute blocks at a time!)
	   
	   need to store a res and a temp for each block
	*/
	for(int i = 0; i < num_cl; i++){
		for(int j = 0; j < 32; j++){ // 32 == num_bits (should be a constant?)
			for(int k = 0; k < 8; k++){ // 8 == blocks per CL .. should be a variable according to blocks per cacheline like in "retrieve_from_simple_mlweaving" .. TODO!!
				// R.a < R.b => a = x, b = y
				a = d[(i * 256) + j * 8 + k];
				b = a >> 1;
				xor = a ^ b;
				/*
				if(i == 2 && j == 31 && k == 0){
					PRINT_64_B(a);
					LINE;
					PRINT_64_B(b);
					LINE;
					PRINT_64_B(xor);
					LINE;
					LINE;
				}*/
				res[k] |= (xor & b) & (~temp[k]);
				temp[k] |= xor & a;
				/*
				if(i == 2 && j == 31 && k == 0){
					PRINT_64_B(res[k]);
					LINE;
				}*/
			}
		}
		
		for(int k = 0; k < 8; k++){
			uint64_t cres = res[k];
			for(int m = 0; m < samples_per_block; m++){
				results[i * samples_per_cl + k * samples_per_block + m] = cres & 1;
				cres = cres >> num_features;
				/*
				if(i == 2 && k == 0){
					PRINT_64_B(cres);
					printf("r: %u idx: %i", results[i * samples_per_cl + k * samples_per_block + m], i * samples_per_cl + k * samples_per_block + m);
					LINE;
				}
				*/
			}
			temp[k] = 0; // reset temp and res
			res[k] = 0;
		}
	}		
}