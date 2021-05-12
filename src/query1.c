
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

// unroll for four samples at once.. bug somewhere
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
					
					size_t c0 = cres_idx;
					size_t c2 = cres_idx + 2;
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