
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query1.h"
#include <math.h>
#include <immintrin.h>
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


// void (uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries){

// }

void q1_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries){
	// printf("Got here q1_weave\n");
	// printf("data=%lx,results=%lx,temps=%lx,wordsize=%d,blocksize=%d,num_fet=%d,num_samples=%d,num_entries=%d\n",data,results,temps,word_size,block_size,num_features,num_samples,number_entries);
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
	// printf("Got here q1_weave2\n");
	// printf("Got here q1_weave2\n");
    // printf("Staring query 1 with:num_blocks=%d,block_size=%d, samples_per_block = %d, samples_per_word = %d,num_features= %d\n",num_blocks,block_size,samples_per_block,samples_per_word,num_features);
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


// can't do block optimization since we load data in series basically (per cache line!)
/*
we have spatial locality (for data and for results / temps)
maybe we can keep a / b in a local variable for an entire chunk_index
compute the feature index cheaper?

maybe can unroll for samples per word

ASSUME 4 FEATURES PER WORD SO IT ALIGNS NICELY.. otherwise a sample over 31 -> 3x bits will break
*/

//easier indexing (no modulo / division)
// data only loaded once per word
void q1_weave_v2(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries){

    int chunk_index;
	int feature_index;
	uint32_t a;
	uint32_t b;
	uint32_t xor;

    int samples_per_word = word_size / num_features;
	int samples_per_block = block_size / num_features;
	// block_size == 512
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
	// rows_per_block == 16
    int cols_per_block = word_size;
	// word size = 32

    for(int k = 0; k < num_blocks;k++){
        for(int j = 0; j < word_size;++j){ 
			for(int m = 0; m < rows_per_block; m++){
				uint32_t load = data[k * block_size + rows_per_block * j  + m];
				for(int i = 0; i < samples_per_word; i++){
					a = (load >> (i * num_features)) & 1;
					b = (load >> (i * num_features + 1)) & 1;
					xor = a ^ b;
					results[k * samples_per_block + i + m * samples_per_word] = results[k * samples_per_block + i + m * samples_per_word] | ((xor & b) & (!temps[k * samples_per_block + i + m * samples_per_word]));
                temps[k * samples_per_block + i + m * samples_per_word] = temps[k * samples_per_block + i + m * samples_per_word] | (xor & a);
					
				}
            } 
	    }
    }
}

// save redoing a bunch of index computations and simplify the mults to adds
void q1_weave_v3(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries){

    int chunk_index;
	int feature_index;
	uint32_t a;
	uint32_t b;
	uint32_t xor;

    int samples_per_word = word_size / num_features;
	int samples_per_block = block_size / num_features;
	// block_size == 512
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
	// rows_per_block == 16
    int cols_per_block = word_size;
	// word size = 32
	
	size_t res_idx = 0; // counts for k loop in samples
	size_t data_index_s = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
		size_t j_idx = 0; // rows per block
        for(int j = 0; j < word_size;++j){
			size_t cres_idx = res_idx; // k * samples_per_block
			for(int m = 0; m < rows_per_block; m++){
				uint32_t load = data[data_index_s + j_idx + m];
				int bit_shift = 0;
				for(int i = 0; i < samples_per_word; i++){
					size_t ci = cres_idx + i;
					a = (load >> (bit_shift)) & 1;
					b = (load >> (bit_shift + 1)) & 1;
					xor = a ^ b;
					results[ci] = results[ci] | ((xor & b) & (!temps[ci]));
                temps[ci] = temps[ci] | (xor & a);
					bit_shift += num_features; // i * num_features
				}
				cres_idx += samples_per_word; // + samples_per_word * m
            }
			j_idx += rows_per_block;	
	    }
		data_index_s += block_size;
		res_idx += samples_per_block;
    }
}

// scalar replacement
void q1_weave_v4(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries){
	/*
	if(num_features % 4 != 0 || num_features > 32){
		printf("can't handle num_features: %i! \n", num_features);
		return;
	}
	*/
    int chunk_index;
	int feature_index;
	uint32_t a;
	uint32_t b;
	uint32_t xor;

    int samples_per_word = word_size / num_features;
	int samples_per_block = block_size / num_features;
	// block_size == 512
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
	// rows_per_block == 16
    int cols_per_block = word_size;
	// word size = 32
	
	size_t res_idx = 0; // counts for k loop in samples
	size_t data_index_s = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
		size_t j_idx = 0; // rows per block
        for(int j = 0; j < word_size;++j){
			size_t cres_idx = res_idx; // k * samples_per_block
			for(int m = 0; m < rows_per_block; m++){
				uint32_t load = data[data_index_s + j_idx + m];
				int bit_shift = 0;
				for(int i = 0; i < samples_per_word; i++){
					
					size_t ci = cres_idx + i;
					uint32_t ctemp = temps[ci];
					
					a = (load >> (bit_shift)) & 1;
					b = (load >> (bit_shift + 1)) & 1;
					xor = a ^ b;
					uint32_t xora = xor & a;
					uint32_t xorb = xor & b;
					uint32_t andnotb = xorb & (!ctemp); 
					results[ci] |= andnotb;
					temps[ci] = ctemp | xora;
					bit_shift += num_features; // i * num_features
				}
				cres_idx += samples_per_word; // + samples_per_word * m
            }
			j_idx += rows_per_block;	
	    }
		data_index_s += block_size;
		res_idx += samples_per_block;
    }
}

// unroll for four samples at once..
// possibly unroll to keep all temps and results in local variables ?
// possible speedup using 64 bit loads instead of 32 ?
void q1_weave_v5(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries){
	
	if(num_features % 4 != 0 || num_features > 32){
		printf("can't handle num_features: %i! \n", num_features);
		return;
	}
	
    int chunk_index;
	int feature_index;

    int samples_per_word = word_size / num_features;
	int samples_per_block = block_size / num_features;
	// block_size == 512
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
	// rows_per_block == 16
    int cols_per_block = word_size;
	// word size = 32
	
	// case where we have 4 or more samples per word and unroll for the word
	if(samples_per_word % 4 == 0 && samples_per_word >= 4){
	size_t res_idx = 0; // counts for k loop in samples
	size_t data_index_s = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
		size_t j_idx = 0; // rows per block
        for(int j = 0; j < word_size;++j){
			size_t cres_idx = res_idx; // k * samples_per_block
			for(int m = 0; m < rows_per_block; m++){
				uint32_t load = data[data_index_s + j_idx + m];
				int bit_shift = 0;
				for(int i = 0; i < samples_per_word; i += 4){
					size_t c0 = cres_idx + i;
					size_t c2 = c0 + 2;
					uint32_t ctemp0 = temps[c0];
					uint32_t ctemp1 = temps[c0 + 1];
					uint32_t ctemp2 = temps[c2];
					uint32_t ctemp3 = temps[c2 + 1];
					
					uint32_t a0 = (load >> (bit_shift)) & 1;
					uint32_t b0 = (load >> (bit_shift + 1)) & 1;
					uint32_t xor0 = a0 ^ b0;
					uint32_t xora0 = xor0 & a0;
					uint32_t xorb0 = xor0 & b0;
					uint32_t andnotb0 = xorb0 & (!ctemp0); 
					results[c0] |= andnotb0;
					temps[c0] = ctemp0 | xora0;
					
					
					uint32_t a1 = (load >> (bit_shift + num_features)) & 1;
					uint32_t b1 = (load >> (bit_shift + num_features + 1)) & 1;
					uint32_t xor1 = a1 ^ b1;
					uint32_t xora1 = xor1 & a1;
					uint32_t xorb1 = xor1 & b1;
					uint32_t andnotb1 = xorb1 & (!ctemp1); 
					results[c0 + 1] |= andnotb1;
					temps[c0 + 1] = ctemp1 | xora1;
					
					int bit_shift2 = bit_shift + 2 * num_features;
					uint32_t a2 = (load >> (bit_shift2)) & 1;
					uint32_t b2 = (load >> (bit_shift2 + 1)) & 1;
					uint32_t xor2 = a2 ^ b2;
					uint32_t xora2 = xor2 & a2;
					uint32_t xorb2 = xor2 & b2;
					uint32_t andnotb2 = xorb2 & (!ctemp2); 
					results[c2] |= andnotb2;
					temps[c2] = ctemp2 | xora2;
					
					
					uint32_t a3 = (load >> (bit_shift2 + num_features)) & 1;
					uint32_t b3 = (load >> (bit_shift2 + num_features + 1)) & 1;
					uint32_t xor3 = a3 ^ b3;
					uint32_t xora3 = xor3 & a3;
					uint32_t xorb3 = xor3 & b3;
					uint32_t andnotb3 = xorb3 & (!ctemp3); 
					results[c2 + 1] |= andnotb3;
					temps[c2 + 1] = ctemp3 | xora3;
					
					
					bit_shift += 4 * num_features; // i * num_features
				}
				cres_idx += samples_per_word; // + samples_per_word * m
            }
			j_idx += rows_per_block;	
	    }
		data_index_s += block_size;
		res_idx += samples_per_block;
    }
	// case of 16 features, unroll two words at once ?
	} else if(samples_per_word == 2){
	size_t res_idx = 0; // counts for k loop in samples
	size_t data_index = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
        for(int j = 0; j < word_size;++j){
			size_t cres_idx = res_idx; // k * samples_per_block
			for(int m = 0; m < rows_per_block; m += 2){
				uint32_t load = data[data_index];
				uint32_t load2 = data[data_index + 1];
				
				size_t c0 = cres_idx;
				size_t c2 = c0 + 2;
				uint32_t ctemp0 = temps[c0];
				uint32_t ctemp1 = temps[c0 + 1];
				uint32_t ctemp2 = temps[c2];
				uint32_t ctemp3 = temps[c2 + 1];
				
				uint32_t a0 = load & 1;
				uint32_t b0 = (load >> 1) & 1;
				uint32_t xor0 = a0 ^ b0;
				uint32_t xora0 = xor0 & a0;
				uint32_t xorb0 = xor0 & b0;
				uint32_t andnotb0 = xorb0 & (!ctemp0); 
				results[c0] |= andnotb0;
				temps[c0] = ctemp0 | xora0;
				
				
				uint32_t a1 = (load >> num_features) & 1;
				uint32_t b1 = (load >> (num_features + 1)) & 1;
				uint32_t xor1 = a1 ^ b1;
				uint32_t xora1 = xor1 & a1;
				uint32_t xorb1 = xor1 & b1;
				uint32_t andnotb1 = xorb1 & (!ctemp1); 
				results[c0 + 1] |= andnotb1;
				temps[c0 + 1] = ctemp1 | xora1;
					
				uint32_t a2 = load2 & 1;
				uint32_t b2 = (load2 >> 1) & 1;
				uint32_t xor2 = a2 ^ b2;
				uint32_t xora2 = xor2 & a2;
				uint32_t xorb2 = xor2 & b2;
				uint32_t andnotb2 = xorb2 & (!ctemp2); 
				results[c2] |= andnotb2;
				temps[c2] = ctemp2 | xora2;
				
				
				uint32_t a3 = (load2 >> (num_features)) & 1;
				uint32_t b3 = (load2 >> (num_features + 1)) & 1;
				uint32_t xor3 = a3 ^ b3;
				uint32_t xora3 = xor3 & a3;
				uint32_t xorb3 = xor3 & b3;
				uint32_t andnotb3 = xorb3 & (!ctemp3); 
				results[c2 + 1] |= andnotb3;
				temps[c2 + 1] = ctemp3 | xora3;
				
				
				cres_idx += 4; // + 2 words worth of 2 samples per word
				data_index += 2;
            }
	    }
		res_idx += samples_per_block;
    }
		
		
	// goal here: UNROLL for every word of the cacheline so we don't keep load ing results and temps
	} else if(samples_per_word == 1){
		//TODO
	size_t res_idx = 0; // counts for k loop in samples
	size_t data_index = 0; // counts k loop in words
	for(int k = 0; k < num_blocks;k++){
		// load all res / temps -> don't need to load initially!
		uint32_t res0 = 0;
		uint32_t res1 = 0;
		uint32_t res2 = 0;
		uint32_t res3 = 0;
		uint32_t res4 = 0;
		uint32_t res5 = 0;
		uint32_t res6 = 0;
		uint32_t res7 = 0;
		
		uint32_t res8 = 0;
		uint32_t res9 = 0;
		uint32_t res10 = 0;
		uint32_t res11 = 0;
		uint32_t res12 = 0;
		uint32_t res13 = 0;
		uint32_t res14 = 0;
		uint32_t res15 = 0;
		
		uint32_t temp0 = 0;
		uint32_t temp1 = 0;
		uint32_t temp2 = 0;
		uint32_t temp3 = 0;
		uint32_t temp4 = 0;
		uint32_t temp5 = 0;
		uint32_t temp6 = 0;
		uint32_t temp7 = 0;
		
		uint32_t temp8 = 0;
		uint32_t temp9 = 0;
		uint32_t temp10 = 0;
		uint32_t temp11 = 0;
		uint32_t temp12 = 0;
		uint32_t temp13 = 0;
		uint32_t temp14 = 0;
		uint32_t temp15 = 0;
		
		
        for(int j = 0; j < word_size;++j){
			uint32_t load0 = data[data_index];
			uint32_t load1 = data[data_index + 1];
			uint32_t load2 = data[data_index + 2];
			uint32_t load3 = data[data_index + 3];
			uint32_t load4 = data[data_index + 4];
			uint32_t load5 = data[data_index + 5];
			uint32_t load6 = data[data_index + 6];
			uint32_t load7 = data[data_index + 7];
			
			uint32_t load8 = data[data_index + 8];
			uint32_t load9 = data[data_index + 9];
			uint32_t load10 = data[data_index + 10];
			uint32_t load11 = data[data_index + 11];
			uint32_t load12 = data[data_index + 12];
			uint32_t load13 = data[data_index + 13];
			uint32_t load14 = data[data_index + 14];
			uint32_t load15 = data[data_index + 15];
			
			uint32_t a0 = load0 & 1;
			uint32_t b0 = (load0 >> 1) & 1;
			uint32_t xor0 = a0 ^ b0;
			uint32_t xora0 = xor0 & a0;
			uint32_t xorb0 = xor0 & b0;
			uint32_t andnotb0 = xorb0 & (!temp0); 
			res0 |= andnotb0;
			temp0 = temp0 | xora0;
			
			uint32_t a1 = load1 & 1;
			uint32_t b1 = (load1 >> 1) & 1;
			uint32_t xor1 = a1 ^ b1;
			uint32_t xora1 = xor1 & a1;
			uint32_t xorb1 = xor1 & b1;
			uint32_t andnotb1 = xorb1 & (!temp1); 
			res1 |= andnotb1;
			temp1 = temp1 | xora1;
			
			uint32_t a2 = load2 & 1;
			uint32_t b2 = (load2 >> 1) & 1;
			uint32_t xor2 = a2 ^ b2;
			uint32_t xora2 = xor2 & a2;
			uint32_t xorb2 = xor2 & b2;
			uint32_t andnotb2 = xorb2 & (!temp2); 
			res2 |= andnotb2;
			temp2 = temp2 | xora2;
			
			uint32_t a3 = load3 & 1;
			uint32_t b3 = (load3 >> 1) & 1;
			uint32_t xor3 = a3 ^ b3;
			uint32_t xora3 = xor3 & a3;
			uint32_t xorb3 = xor3 & b3;
			uint32_t andnotb3 = xorb3 & (!temp3); 
			res3 |= andnotb3;
			temp3 = temp3 | xora3;
			
			uint32_t a4 = load4 & 1;
			uint32_t b4 = (load4 >> 1) & 1;
			uint32_t xor4 = a4 ^ b4;
			uint32_t xora4 = xor4 & a4;
			uint32_t xorb4 = xor4 & b4;
			uint32_t andnotb4 = xorb4 & (!temp4); 
			res4 |= andnotb4;
			temp4 = temp4 | xora4;
			
			uint32_t a5 = load5 & 1;
			uint32_t b5 = (load5 >> 1) & 1;
			uint32_t xor5 = a5 ^ b5;
			uint32_t xora5 = xor5 & a5;
			uint32_t xorb5 = xor5 & b5;
			uint32_t andnotb5 = xorb5 & (!temp5); 
			res5 |= andnotb5;
			temp5 = temp5 | xora5;
			
			uint32_t a6 = load6 & 1;
			uint32_t b6 = (load6 >> 1) & 1;
			uint32_t xor6 = a6 ^ b6;
			uint32_t xora6 = xor6 & a6;
			uint32_t xorb6 = xor6 & b6;
			uint32_t andnotb6 = xorb6 & (!temp6); 
			res6 |= andnotb6;
			temp6 = temp6 | xora6;
			
			uint32_t a7 = load7 & 1;
			uint32_t b7 = (load7 >> 1) & 1;
			uint32_t xor7 = a7 ^ b7;
			uint32_t xora7 = xor7 & a7;
			uint32_t xorb7 = xor7 & b7;
			uint32_t andnotb7 = xorb7 & (!temp7); 
			res7 |= andnotb7;
			temp7 = temp7 | xora7;
			
			
			uint32_t a8 = load8 & 1;
			uint32_t b8 = (load8 >> 1) & 1;
			uint32_t xor8 = a8 ^ b8;
			uint32_t xora8 = xor8 & a8;
			uint32_t xorb8 = xor8 & b8;
			uint32_t andnotb8 = xorb8 & (!temp8); 
			res8 |= andnotb8;
			temp8 = temp8 | xora8;
			
			uint32_t a9 = load9 & 1;
			uint32_t b9 = (load9 >> 1) & 1;
			uint32_t xor9 = a9 ^ b9;
			uint32_t xora9 = xor9 & a9;
			uint32_t xorb9 = xor9 & b9;
			uint32_t andnotb9 = xorb9 & (!temp9); 
			res9 |= andnotb9;
			temp9 = temp9 | xora9;
			
			uint32_t a10 = load10 & 1;
			uint32_t b10 = (load10 >> 1) & 1;
			uint32_t xor10 = a10 ^ b10;
			uint32_t xora10 = xor10 & a10;
			uint32_t xorb10 = xor10 & b10;
			uint32_t andnotb10 = xorb10 & (!temp10); 
			res10 |= andnotb10;
			temp10 = temp10 | xora10;
			
			uint32_t a11 = load11 & 1;
			uint32_t b11 = (load11 >> 1) & 1;
			uint32_t xor11 = a11 ^ b11;
			uint32_t xora11 = xor11 & a11;
			uint32_t xorb11 = xor11 & b11;
			uint32_t andnotb11 = xorb11 & (!temp11); 
			res11 |= andnotb11;
			temp11 = temp11 | xora11;
			
			uint32_t a12 = load12 & 1;
			uint32_t b12 = (load12 >> 1) & 1;
			uint32_t xor12 = a12 ^ b12;
			uint32_t xora12 = xor12 & a12;
			uint32_t xorb12 = xor12 & b12;
			uint32_t andnotb12 = xorb12 & (!temp12); 
			res12 |= andnotb12;
			temp12 = temp12 | xora12;
			
			uint32_t a13 = load13 & 1;
			uint32_t b13 = (load13 >> 1) & 1;
			uint32_t xor13 = a13 ^ b13;
			uint32_t xora13 = xor13 & a13;
			uint32_t xorb13 = xor13 & b13;
			uint32_t andnotb13 = xorb13 & (!temp13); 
			res13 |= andnotb13;
			temp13 = temp13 | xora13;
			
			uint32_t a14 = load14 & 1;
			uint32_t b14 = (load14 >> 1) & 1;
			uint32_t xor14 = a14 ^ b14;
			uint32_t xora14 = xor14 & a14;
			uint32_t xorb14 = xor14 & b14;
			uint32_t andnotb14 = xorb14 & (!temp14); 
			res14 |= andnotb14;
			temp14 = temp14 | xora14;
			
			uint32_t a15 = load15 & 1;
			uint32_t b15 = (load15 >> 1) & 1;
			uint32_t xor15 = a15 ^ b15;
			uint32_t xora15 = xor15 & a15;
			uint32_t xorb15 = xor15 & b15;
			uint32_t andnotb15 = xorb15 & (!temp15); 
			res15 |= andnotb15;
			temp15 = temp15 | xora15;
			
			data_index += 16; //words per cacheline 
		}
		
		results[res_idx] = res0;
		results[res_idx + 1] = res1;
		results[res_idx + 2] = res2;
		results[res_idx + 3] = res3;
		results[res_idx + 4] = res4;
		results[res_idx + 5] = res5;
		results[res_idx + 6] = res6;
		results[res_idx + 7] = res7;
		
		results[res_idx + 8] = res8;
		results[res_idx + 9] = res9;
		results[res_idx + 10] = res10;
		results[res_idx + 11] = res11;
		results[res_idx + 12] = res12;
		results[res_idx + 13] = res13;
		results[res_idx + 14] = res14;
		results[res_idx + 15] = res15;
	res_idx += 16; // samples per block
	
	}
	}
}

// POSSIBLE NEXT OPTIMIZATION: CHANGE DATA TYPES!
/*
res can be either a bitvector or at least a vector of chars

temp also only needs to be a char array (only need one bit!)

then can optimize parallel & vector

NEGLIGIBLE IMPROVEMENT OVER v5, maybe just remove this one
*/
void q1_weave_v6(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries){
	
	if(num_features % 4 != 0 || num_features > 32){
		printf("can't handle num_features: %i! \n", num_features);
		return;
	}
	
    int chunk_index;
	int feature_index;

    int samples_per_word = 64 / num_features;
	int samples_per_block = block_size / num_features;
	// block_size == 512
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / 64;
	// rows_per_block == 8
    int cols_per_block = word_size;
	// word size = 32
	
	//printf("spw: %i, spb: %i, nb: %i, cpb: %i \n", samples_per_word, samples_per_block, num_blocks, cols_per_block);
	
	uint64_t* d = data; // use 64 bit words!
	
	// case where we have 4 or more samples per word and unroll for the word
	if(samples_per_word % 4 == 0 && samples_per_word > 4){
	size_t res_idx = 0; // counts for k loop in samples
	size_t data_index_s = 0; // counts k loop in words
	
	
    for(int k = 0; k < num_blocks;k++){
		size_t j_idx = 0; // rows per block
        for(int j = 0; j < word_size;++j){
			size_t cres_idx = res_idx; // k * samples_per_block
			for(int m = 0; m < rows_per_block; m++){
				uint64_t load = d[data_index_s + j_idx + m];
				unsigned char bit_shift = 0;
				for(int i = 0; i < samples_per_word; i += 4){
					size_t c0 = cres_idx + i;
					size_t c2 = c0 + 2;
					unsigned char ctemp0 = temps[c0];
					unsigned char ctemp1 = temps[c0 + 1];
					unsigned char ctemp2 = temps[c2];
					unsigned char ctemp3 = temps[c2 + 1];
					
					unsigned char a0 = (load >> (bit_shift)) & 1;
					unsigned char b0 = (load >> (bit_shift + 1)) & 1;
					unsigned char xor0 = a0 ^ b0;
					unsigned char xora0 = xor0 & a0;
					unsigned char xorb0 = xor0 & b0;
					unsigned char andnotb0 = xorb0 & (!ctemp0); 
					results[c0] |= andnotb0;
					temps[c0] = ctemp0 | xora0;
					
					
					unsigned char a1 = (load >> (bit_shift + num_features)) & 1;
					unsigned char b1 = (load >> (bit_shift + num_features + 1)) & 1;
					unsigned char xor1 = a1 ^ b1;
					unsigned char xora1 = xor1 & a1;
					unsigned char xorb1 = xor1 & b1;
					unsigned char andnotb1 = xorb1 & (!ctemp1); 
					results[c0 + 1] |= andnotb1;
					temps[c0 + 1] = ctemp1 | xora1;
					
					unsigned char bit_shift2 = bit_shift + 2 * num_features;
					unsigned char a2 = (load >> (bit_shift2)) & 1;
					unsigned char b2 = (load >> (bit_shift2 + 1)) & 1;
					unsigned char xor2 = a2 ^ b2;
					unsigned char xora2 = xor2 & a2;
					unsigned char xorb2 = xor2 & b2;
					unsigned char andnotb2 = xorb2 & (!ctemp2); 
					results[c2] |= andnotb2;
					temps[c2] = ctemp2 | xora2;
					
					
					unsigned char a3 = (load >> (bit_shift2 + num_features)) & 1;
					unsigned char b3 = (load >> (bit_shift2 + num_features + 1)) & 1;
					unsigned char xor3 = a3 ^ b3;
					unsigned char xora3 = xor3 & a3;
					unsigned char xorb3 = xor3 & b3;
					unsigned char andnotb3 = xorb3 & (!ctemp3); 
					results[c2 + 1] |= andnotb3;
					temps[c2 + 1] = ctemp3 | xora3;
					
					
					bit_shift += 4 * num_features; // i * num_features
				}
				cres_idx += samples_per_word; // + samples_per_word * m
            }
			j_idx += rows_per_block;	
	    }
		data_index_s += 256;
		res_idx += samples_per_block;
    }
	// case of 16 features, unroll FOUR words at once ?
	} else if(samples_per_word == 4){
	size_t res_idx = 0; // counts for k loop in samples
	size_t data_index = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
        for(int j = 0; j < word_size;++j){
			size_t cres_idx = res_idx; // k * samples_per_block
			for(int m = 0; m < rows_per_block; m += 2){
				uint64_t load = d[data_index];
				uint64_t load2 = d[data_index + 1];
				
				size_t c0 = cres_idx;
				size_t c2 = c0 + 2;
				size_t c4 = c0 + 4;
				size_t c6 = c0 + 6;
				unsigned char ctemp0 = temps[c0];
				unsigned char ctemp1 = temps[c0 + 1];
				unsigned char ctemp2 = temps[c2];
				unsigned char ctemp3 = temps[c2 + 1];
				unsigned char ctemp4 = temps[c4];
				unsigned char ctemp5 = temps[c4 + 1];
				unsigned char ctemp6 = temps[c6];
				unsigned char ctemp7 = temps[c6 + 1];
				
				unsigned char a0 = load & 1;
				unsigned char b0 = (load >> 1) & 1;
				unsigned char xor0 = a0 ^ b0;
				unsigned char xora0 = xor0 & a0;
				unsigned char xorb0 = xor0 & b0;
				unsigned char andnotb0 = xorb0 & (!ctemp0); 
				results[c0] |= andnotb0;
				temps[c0] = ctemp0 | xora0;
				
				
				unsigned char a1 = (load >> num_features) & 1;
				unsigned char b1 = (load >> (num_features + 1)) & 1;
				unsigned char xor1 = a1 ^ b1;
				unsigned char xora1 = xor1 & a1;
				unsigned char xorb1 = xor1 & b1;
				unsigned char andnotb1 = xorb1 & (!ctemp1); 
				results[c0 + 1] |= andnotb1;
				temps[c0 + 1] = ctemp1 | xora1;
				
				unsigned char nf2 = 2 * num_features;
				unsigned char a2 = (load >> nf2) & 1;
				unsigned char b2 = (load >> (nf2 + 1)) & 1;
				unsigned char xor2 = a2 ^ b2;
				unsigned char xora2 = xor2 & a2;
				unsigned char xorb2 = xor2 & b2;
				unsigned char andnotb2 = xorb2 & (!ctemp2); 
				results[c2] |= andnotb2;
				temps[c2] = ctemp2 | xora2;
				
				
				unsigned char a3 = (load >> (nf2 + num_features)) & 1;
				unsigned char b3 = (load >> (nf2 + 1 + num_features)) & 1;
				unsigned char xor3 = a3 ^ b3;
				unsigned char xora3 = xor3 & a3;
				unsigned char xorb3 = xor3 & b3;
				unsigned char andnotb3 = xorb3 & (!ctemp3); 
				results[c2 + 1] |= andnotb3;
				temps[c2 + 1] = ctemp3 | xora3;
				
				
				//---------
				
				
				unsigned char a4 = load2 & 1;
				unsigned char b4 = (load2 >> 1) & 1;
				unsigned char xor4 = a4 ^ b4;
				unsigned char xora4 = xor4 & a4;
				unsigned char xorb4 = xor4 & b4;
				unsigned char andnotb4 = xorb4 & (!ctemp4); 
				results[c4] |= andnotb4;
				temps[c4] = ctemp4 | xora4;
				
				
				unsigned char a5 = (load2 >> num_features) & 1;
				unsigned char b5 = (load2 >> (num_features + 1)) & 1;
				unsigned char xor5 = a5 ^ b5;
				unsigned char xora5 = xor5 & a5;
				unsigned char xorb5 = xor5 & b5;
				unsigned char andnotb5 = xorb5 & (!ctemp5); 
				results[c4 + 1] |= andnotb5;
				temps[c4 + 1] = ctemp5 | xora5;
				
				unsigned char a6 = (load2 >> nf2) & 1;
				unsigned char b6 = (load2 >> (nf2 + 1)) & 1;
				unsigned char xor6 = a6 ^ b6;
				unsigned char xora6 = xor6 & a6;
				unsigned char xorb6 = xor6 & b6;
				unsigned char andnotb6 = xorb6 & (!ctemp6); 
				results[c6] |= andnotb6;
				temps[c6] = ctemp6 | xora6;
				
				
				unsigned char a7 = (load2 >> (nf2 + num_features)) & 1;
				unsigned char b7 = (load2 >> (nf2 + 1 + num_features)) & 1;
				unsigned char xor7 = a7 ^ b7;
				unsigned char xora7 = xor7 & a7;
				unsigned char xorb7 = xor7 & b7;
				unsigned char andnotb7 = xorb7 & (!ctemp7); 
				results[c6 + 1] |= andnotb7;
				temps[c6 + 1] = ctemp7 | xora7;
				
				
				cres_idx += 8; // + 2 words worth of 4 samples per word
				data_index += 2;
            }
	    }
		res_idx += samples_per_block;
    }
		
		
	// goal here: UNROLL for every word of the cacheline so we don't keep load ing results and temps
	} else if(samples_per_word == 2){
	size_t res_idx = 0; // counts for k loop in samples
	size_t data_index = 0; // counts k loop in words
	for(int k = 0; k < num_blocks;k++){
		// load all res / temps -> don't need to load initially!
		unsigned char res0 = 0;
		unsigned char res1 = 0;
		unsigned char res2 = 0;
		unsigned char res3 = 0;
		unsigned char res4 = 0;
		unsigned char res5 = 0;
		unsigned char res6 = 0;
		unsigned char res7 = 0;
		unsigned char res01 = 0;
		unsigned char res11 = 0;
		unsigned char res21 = 0;
		unsigned char res31 = 0;
		unsigned char res41 = 0;
		unsigned char res51 = 0;
		unsigned char res61 = 0;
		unsigned char res71 = 0;
		
		
		unsigned char temp0 = 0;
		unsigned char temp1 = 0;
		unsigned char temp2 = 0;
		unsigned char temp3 = 0;
		unsigned char temp4 = 0;
		unsigned char temp5 = 0;
		unsigned char temp6 = 0;
		unsigned char temp7 = 0;
		unsigned char temp01 = 0;
		unsigned char temp11 = 0;
		unsigned char temp21 = 0;
		unsigned char temp31 = 0;
		unsigned char temp41 = 0;
		unsigned char temp51 = 0;
		unsigned char temp61 = 0;
		unsigned char temp71 = 0;
		
		
        for(int j = 0; j < word_size;++j){
			uint64_t load0 = d[data_index];
			uint64_t load1 = d[data_index + 1];
			uint64_t load2 = d[data_index + 2];
			uint64_t load3 = d[data_index + 3];
			uint64_t load4 = d[data_index + 4];
			uint64_t load5 = d[data_index + 5];
			uint64_t load6 = d[data_index + 6];
			uint64_t load7 = d[data_index + 7];
			
			unsigned char a0 = load0 & 1;
			unsigned char b0 = (load0 >> 1) & 1;
			unsigned char xor0 = a0 ^ b0;
			unsigned char xora0 = xor0 & a0;
			unsigned char xorb0 = xor0 & b0;
			unsigned char andnotb0 = xorb0 & (!temp0); 
			res0 |= andnotb0;
			temp0 = temp0 | xora0;
			
			unsigned char a01 = (load0 >> num_features) & 1;
			unsigned char b01 = (load0 >> (num_features + 1)) & 1;
			unsigned char xor01 = a01 ^ b01;
			unsigned char xora01 = xor01 & a01;
			unsigned char xorb01 = xor01 & b01;
			unsigned char andnotb01 = xorb01 & (!temp01); 
			res01 |= andnotb01;
			temp01 = temp01 | xora01;
			
			unsigned char a1 = load1 & 1;
			unsigned char b1 = (load1 >> 1) & 1;
			unsigned char xor1 = a1 ^ b1;
			unsigned char xora1 = xor1 & a1;
			unsigned char xorb1 = xor1 & b1;
			unsigned char andnotb1 = xorb1 & (!temp1); 
			res1 |= andnotb1;
			temp1 = temp1 | xora1;
			
			unsigned char a11 = (load1 >> num_features) & 1;
			unsigned char b11 = (load1 >> (num_features + 1)) & 1;
			unsigned char xor11 = a11 ^ b11;
			unsigned char xora11 = xor11 & a11;
			unsigned char xorb11 = xor11 & b11;
			unsigned char andnotb11 = xorb11 & (!temp11); 
			res11 |= andnotb11;
			temp11 = temp11 | xora11;
			
			unsigned char a2 = load2 & 1;
			unsigned char b2 = (load2 >> 1) & 1;
			unsigned char xor2 = a2 ^ b2;
			unsigned char xora2 = xor2 & a2;
			unsigned char xorb2 = xor2 & b2;
			unsigned char andnotb2 = xorb2 & (!temp2); 
			res2 |= andnotb2;
			temp2 = temp2 | xora2;
			
			unsigned char a21 = (load2 >> num_features) & 1;
			unsigned char b21 = (load2 >> (num_features + 1)) & 1;
			unsigned char xor21 = a21 ^ b21;
			unsigned char xora21 = xor21 & a21;
			unsigned char xorb21 = xor21 & b21;
			unsigned char andnotb21 = xorb21 & (!temp21); 
			res21 |= andnotb21;
			temp21 = temp21 | xora21;
			
			unsigned char a3 = load3 & 1;
			unsigned char b3 = (load3 >> 1) & 1;
			unsigned char xor3 = a3 ^ b3;
			unsigned char xora3 = xor3 & a3;
			unsigned char xorb3 = xor3 & b3;
			unsigned char andnotb3 = xorb3 & (!temp3); 
			res3 |= andnotb3;
			temp3 = temp3 | xora3;
			
			unsigned char a31 = (load3 >> num_features) & 1;
			unsigned char b31 = (load3 >> (num_features + 1)) & 1;
			unsigned char xor31 = a31 ^ b31;
			unsigned char xora31 = xor31 & a31;
			unsigned char xorb31 = xor31 & b31;
			unsigned char andnotb31 = xorb31 & (!temp31); 
			res31 |= andnotb31;
			temp31 = temp31 | xora31;
			
			unsigned char a4 = load4 & 1;
			unsigned char b4 = (load4 >> 1) & 1;
			unsigned char xor4 = a4 ^ b4;
			unsigned char xora4 = xor4 & a4;
			unsigned char xorb4 = xor4 & b4;
			unsigned char andnotb4 = xorb4 & (!temp4); 
			res4 |= andnotb4;
			temp4 = temp4 | xora4;
			
			unsigned char a41 = (load4 >> num_features) & 1;
			unsigned char b41 = (load4 >> (num_features + 1)) & 1;
			unsigned char xor41 = a41 ^ b41;
			unsigned char xora41 = xor41 & a41;
			unsigned char xorb41 = xor41 & b41;
			unsigned char andnotb41 = xorb41 & (!temp41); 
			res41 |= andnotb41;
			temp41 = temp41 | xora41;
			
			unsigned char a5 = load5 & 1;
			unsigned char b5 = (load5 >> 1) & 1;
			unsigned char xor5 = a5 ^ b5;
			unsigned char xora5 = xor5 & a5;
			unsigned char xorb5 = xor5 & b5;
			unsigned char andnotb5 = xorb5 & (!temp5); 
			res5 |= andnotb5;
			temp5 = temp5 | xora5;
			
			unsigned char a51 = (load5 >> num_features) & 1;
			unsigned char b51 = (load5 >> (num_features + 1)) & 1;
			unsigned char xor51 = a51 ^ b51;
			unsigned char xora51 = xor51 & a51;
			unsigned char xorb51 = xor51 & b51;
			unsigned char andnotb51 = xorb51 & (!temp51); 
			res51 |= andnotb51;
			temp51 = temp51 | xora51;
			
			unsigned char a6 = load6 & 1;
			unsigned char b6 = (load6 >> 1) & 1;
			unsigned char xor6 = a6 ^ b6;
			unsigned char xora6 = xor6 & a6;
			unsigned char xorb6 = xor6 & b6;
			unsigned char andnotb6 = xorb6 & (!temp6); 
			res6 |= andnotb6;
			temp6 = temp6 | xora6;
			
			unsigned char a61 = (load6 >> num_features) & 1;
			unsigned char b61 = (load6 >> (num_features + 1)) & 1;
			unsigned char xor61 = a61 ^ b61;
			unsigned char xora61 = xor61 & a61;
			unsigned char xorb61 = xor61 & b61;
			unsigned char andnotb61 = xorb61 & (!temp61); 
			res61 |= andnotb61;
			temp61 = temp61 | xora61;
			
			unsigned char a7 = load7 & 1;
			unsigned char b7 = (load7 >> 1) & 1;
			unsigned char xor7 = a7 ^ b7;
			unsigned char xora7 = xor7 & a7;
			unsigned char xorb7 = xor7 & b7;
			unsigned char andnotb7 = xorb7 & (!temp7); 
			res7 |= andnotb7;
			temp7 = temp7 | xora7;
			
			unsigned char a71 = (load7 >> num_features) & 1;
			unsigned char b71 = (load7 >> (num_features + 1)) & 1;
			unsigned char xor71 = a71 ^ b71;
			unsigned char xora71 = xor71 & a71;
			unsigned char xorb71 = xor71 & b71;
			unsigned char andnotb71 = xorb71 & (!temp71); 
			res71 |= andnotb71;
			temp71 = temp71 | xora71;
			
			data_index += 8; //words per cacheline 
		}
		
		results[res_idx] = res0;
		results[res_idx + 1] = res01;
		results[res_idx + 2] = res1;
		results[res_idx + 3] = res11;
		results[res_idx + 4] = res2;
		results[res_idx + 5] = res21;
		results[res_idx + 6] = res3;
		results[res_idx + 7] = res31;
		
		results[res_idx + 8] = res4;
		results[res_idx + 9] = res41;
		results[res_idx + 10] = res5;
		results[res_idx + 11] = res51;
		results[res_idx + 12] = res6;
		results[res_idx + 13] = res61;
		results[res_idx + 14] = res7;
		results[res_idx + 15] = res71;
	res_idx += 16; // samples per block
	
	}
	}
}



void q1_parallel_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries){
	
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

/*
possible optimizations:
loop unrolling (unroll k loop at least)
scalar replacement -> combine the two k loops, only store res once
   ^-> also temp scalar replacement
make res a bitvector ?


EARLY PRUNING OPTIMIZATION ! (also for the earlier ones!)
for unrolled ones, can keep a counter (how many are decided, i.e. res == 1? see if an if statement test on counter is not worth possible early pruning (probably depends on the scale of the values)

can't do any blocking work, given the way the data is layed out
at best can try and keep RES/TMP in L1 cache for the non-parallel implementations
*/

// seems to work now, could use some cleaning up.. validation crashes but single test seems good?

/* must be corrupting memory somewhere, things i have tested:
- memcpy (to load the data into cres)
- results somehow writing 64bits
- index out of bound of results
- commenting out everything after computing res1 and 2 (i.e. ~line 240)

- COMMENTING OUT THE j LOOP FIXES CRASH
- memory corruption happens somewhere in j loop! (wtf?!)
- it is 100% the vector load intrinsics causing the corruption / crash
- was using ALIGNED load, but data was not 32bit aligned !!

OP COUNT (vector instructions count as 4 ops ? or 8? depends how you count 32 vs 64 bit words in comparison to parallel weave it would be 4 as parallel uses 64bit words):
(not counting working out samples per block and similar things)
i loop:
	j loop: (happens num_cl * 32 times), all ops are 256bit vector ops !
		2 right shifts
		2 xor
		4 or
		4 and
		2 and / not (counts as one op ? two?)
		
	m loop: (happens num_cl * samples_per_block * 2 times (two vectors per i loop))
		4 and
		4 right shifts
		
		
NOTE:
SHOULD MAKE THE READ OUT INTO RESULTS MORE MODULAR TO AVOID WRITING BEYOND THE ARRAY IF THERE ARENT enough samples to fill out all of the blocks in every cacheline block.
either that, or pad the results array to align with the numper of samples per cacheline block




possible optimizations:
aligned loop?
unroll for j? (worth a try see if it speeds it up)
maybe make the read out more specific (i.e. no "for" statement if only one sample per block)
also then can unroll the read out somewhat accordingly perhaps
check if there are alternatives to memcpy which might be faster?
is setzero 1 cycle?
simplify the index operations (remove multiplier?) -> can do one scalar replace since it is reused

maybe to read results, can use upper / lower loads and do it in parallel too ??
maybe there is a mask load so i only load specific bits? (could then just run through those?)
if that is the case -> make results a bit string ! just keep loading howevermany samples per word using mask load? (maybe this doesn't exist but it's an idea)

check if memory bound somehow? try to compute ops, find bandwidth etc

if there is only one sample per word, could non vector be faster?


POSSIBLE INTERESTING EXTENSION: make it more than 64 features per sample.. compare one feature from the first block with one feature from the second block (i.e. CL block)

but then really you would just have to load both blocks, not much different.. can't really block out the memory use any better

at best some register optimization perhaps ? try to limit to 16 registers we reuse? sort of bolck it out that way?

*/
void q1_vector_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples, int num_features,int number_entries){

	__m256i a1;
	__m256i b1;
	__m256i xor1;
	__m256i a2;
	__m256i b2;
	__m256i xor2;
	
	__m256i res1 = _mm256_setzero_si256();
	__m256i res2 = _mm256_setzero_si256();
	
	__m256i temp1 = _mm256_setzero_si256();
	__m256i temp2 = _mm256_setzero_si256();
	
	int samples_per_block = 64 / num_features;
	int samples_per_cl = samples_per_block * 8;
   	int num_cl = ceil(((float)num_samples) / samples_per_cl);
	
	uint64_t * d = data;
	
	for(int i = 0; i < num_cl; i++){    // cacheline block index -> 256 64bit words per cacheline block
		for(int j = 0; j < 32; j++){    // 32bit words -> 8 64bit words per cacheline (i.e. 256 samples bits)
			
			//UNROLL LOOP FOR BOTH VECTORS, just easier than an array of vectors or something
			// load correct vector
			a1 = (__m256i) _mm256_loadu_pd(d + (i * 256) + j * 8);
			
			// GOOD WAY TO INSPECT __mXXX values
			/*
			if(i == 0 && j > 16){
				int64_t v64val[4];
				memcpy(v64val, &a1, sizeof(v64val));
				PRINT_64_B(v64val[3]);
				LINE;
			}
			*/
			
			// shift right by one
			b1 = _mm256_srli_epi64(a1, 1);
			//xor 
			xor1 = _mm256_xor_si256(a1, b1);
			
			//compute res
			b1 = _mm256_and_si256 (xor1, b1); //xor & b
			b1 = _mm256_andnot_si256 (temp1, b1); //(xor & b) & (~temp)
			res1 = _mm256_or_si256 (res1, b1); // res |= ^
			
			//compute temp
			a1 = _mm256_and_si256 (a1, xor1);
			temp1 = _mm256_or_si256 (temp1, a1);
			
			//now for second vector of the cache line
			// load correct vector
			a2 = (__m256i) _mm256_loadu_ps(d + (i * 256) + j * 8 + 4); // + 4 as 4 64 bit words per vector
			
			// shift right by one
			b2 = _mm256_srli_epi64(a2, 1);
			//xor 
			xor2 = _mm256_xor_si256(a2, b2);
			
			//compute res
			b2 = _mm256_and_si256 (xor2, b2); //xor & b
			b2 = _mm256_andnot_si256 (temp2, b2); //(xor & b) & (~temp)
			res2 = _mm256_or_si256 (res2, b2); // res |= ^
			
			//compute temp
			a2 = _mm256_and_si256 (a2, xor2);
			temp2 = _mm256_or_si256 (temp2, a2);
			
		}
		
		//printf("reach here");
		// read results out 
		uint64_t cres[4];
		memcpy(cres, &res1, sizeof(cres));
		//_mm256_store_pd(cres,(__m256d) res1); // causes crash for some reason.. investigate!
		
		uint64_t cres0 = cres[0];
		uint64_t cres1 = cres[1];
		uint64_t cres2 = cres[2];
		uint64_t cres3 = cres[3];
		/*
		if(i == 0){
			printf("c0, c1, c2, c3 (in order): \n");
			PRINT_64_B(cres[0]);
			LINE;
			PRINT_64_B(cres[1]);
			LINE;
			PRINT_64_B(cres[2]);
			LINE;
			PRINT_64_B(cres[3]);
			LINE;
		}
		*/
		for(int m = 0; m < samples_per_block; m++){
			// first 64 bit block
			results[i * samples_per_cl + m] = cres0 & 1;
			cres0 = cres0 >> num_features;
			
			// second 64 bit block
			results[i * samples_per_cl + samples_per_block + m] = cres1 & 1;
			cres1 = cres1 >> num_features;
			
			//third
			results[i * samples_per_cl + 2 * samples_per_block + m] = cres2 & 1;
			cres2 = cres2 >> num_features;
			
			
			//fourth
			results[i * samples_per_cl + 3 * samples_per_block + m] = cres3 & 1;
			cres3 = cres3 >> num_features;
		}
		
		// NOW SAME THING BUT FOR res2 !
		
		// read results out 
		uint64_t cres_2[4];
		memcpy(cres_2, &res2, sizeof(cres_2));
		uint64_t cres02 = cres_2[0];
		uint64_t cres12 = cres_2[1];
		uint64_t cres22 = cres_2[2];
		uint64_t cres32 = cres_2[3];
		for(int m = 0; m < samples_per_block; m++){
			// first 64 bit block
			results[i * samples_per_cl + m + 4 * samples_per_block] = cres02 & 1;
			cres02 = cres02 >> num_features;
			
			// second 64 bit block
			results[i * samples_per_cl + (4 + 1) * samples_per_block + m] = cres12 & 1;
			cres12 = cres12 >> num_features;
			
			//third
			results[i * samples_per_cl + (4 + 2) * samples_per_block + m] = cres22 & 1;
			cres22 = cres22 >> num_features;
			
			
			//fourth
			results[i * samples_per_cl + (4 + 3) * samples_per_block + m] = cres32 & 1;
			cres32 = cres32 >> num_features;
			//printf("SAMPLE NR: %i \n", i * samples_per_cl + (4 + 3) * samples_per_block + m);
		}
		
		// reset temp and res
		res1 = _mm256_setzero_si256();
		res2 = _mm256_setzero_si256();
		
		temp1 = _mm256_setzero_si256();
		temp2 = _mm256_setzero_si256();
	}
}