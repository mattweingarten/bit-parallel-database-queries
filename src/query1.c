
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query1.h"
#include <math.h>
#include <x86intrin.h>
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

// STILL BUGGY FOR SOME REASON (crash as we get into the loop,, maybe the load is borked)

void q1_vector_weave(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_features, int num_samples,int number_entries){
	
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
	
	printf("num_cl: %i ", num_cl);
	printf("REACH HERE\n\n");
	uint64_t * d = data;
	
	
	for(int i = 0; i < num_cl; i++){    // cacheline block index -> 256 64bit words per cacheline block
		for(int j = 0; j < 32; j++){    // 32bit words -> 8 64bit words per cacheline (i.e. 256 samples bits)
			
			//UNROLL LOOP FOR BOTH VECTORS, just easier than an array of vectors or something
			// load correct vector
			a1 = _mm256_load_si256((__m256i *) (d + (i * 256) + j * 8));
			
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
			a2 = (__m256i) _mm256_load_ps(d + (i * 256) + j * 8 + 4); // + 4 as 4 64 bit words per vector
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
		
		// can't think of a way of doing it with just one shift for now
		
		// read results out 
		uint64_t cres0 = (uint64_t) _mm256_cvtsd_f64((__m256d)res1); // convert to double
		__m256i res1s = _mm256_srli_si256(res1, 64); //shift by 64 bit
		uint64_t cres1 = (uint64_t) _mm256_cvtsd_f64((__m256d)res1s); // load new bottom 64 bit
		
		__m128i res1_u = _mm256_extractf128_si256(res1, 1); // load top 128 bits
		
		// load next 128 bits (at most 64 bits per sample, so will always need to read every 64bit word)
		uint64_t cres2 = (uint64_t) _mm_cvtsd_f64 ((__m128d)res1_u);
		__m128i res1_us = _mm_srli_si128 (res1_u, 64); //shift by 64 bit
		uint64_t cres3 = (uint64_t) _mm_cvtsd_f64 ((__m128d)res1_us);
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
		cres0 = (uint64_t) _mm256_cvtsd_f64((__m256d)res2); // convert to double
		res1s = _mm256_srli_si256(res1, 64); //shift by 64 bit
		cres1 = (uint64_t) _mm256_cvtsd_f64((__m256d)res1s); // load new bottom 64 bit
		
		res1_u = _mm256_extractf128_si256(res1, 1); // load top 128 bits
		
		// load next 128 bits (at most 64 bits per sample, so will always need to read every 64bit word)
		cres2 = (uint64_t) _mm_cvtsd_f64 ((__m128d)res1_u);
		res1_us = _mm_srli_si128 (res1_u, 64); //shift by 64 bit
		cres3 = (uint64_t) _mm_cvtsd_f64 ((__m128d)res1_us);
		for(int m = 0; m < samples_per_block; m++){
			// first 64 bit block
			results[i * samples_per_cl + m + 4 * samples_per_block] = cres0 & 1;
			cres0 = cres0 >> num_features;
			
			// second 64 bit block
			results[i * samples_per_cl + (4 + 1) * samples_per_block + m] = cres1 & 1;
			cres1 = cres1 >> num_features;
			
			//third
			results[i * samples_per_cl + (4 + 2) * samples_per_block + m] = cres2 & 1;
			cres2 = cres2 >> num_features;
			
			
			//fourth
			results[i * samples_per_cl + (4 + 3) * samples_per_block + m] = cres3 & 1;
			cres3 = cres3 >> num_features;
		}
		
		// reset temp and res
		res1 = _mm256_setzero_si256();
		res2 = _mm256_setzero_si256();
		
		temp1 = _mm256_setzero_si256();
		temp2 = _mm256_setzero_si256();
	}
	
	
	
	
}