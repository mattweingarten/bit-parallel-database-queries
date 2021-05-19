
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query3.h"

#include <immintrin.h>



//SELECT * FROM R, S WHERE R.a % S.b = S.c;

//Straightforward

// always assuming R.a == first feature of R
// S.b == second feature of S, S.c == third feature of S
// dest should be R_rows * S_rows long, 2d array, indexing which combinations of R and S to output
// we assume the output to be atleast of size (R_rows * S_rows) * (R_cols + S_cols)
void q3(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, int R_rows, int R_cols, int S_rows, int S_cols){
	size_t dest_row = 0;
	size_t dest_cols = R_cols + S_cols;
	int k;
	int l;
    for(int i = 0; i < R_rows; i++){
		for(int j = 0; j < S_rows; j++){
			uint32_t R_a = dR[i * R_cols + 0];
			uint32_t S_b = dS[j * S_cols + 1];
			uint32_t S_c = dS[j * S_cols + 2];
			// printf("R[%d],S[%d]: %u mod %u  == %u|",i,j,R_a,S_b,S_c);
			if(S_b != 0 && R_a % S_b == S_c){
				k = 0;
				l = 0;
				// printf("%d => R[%d],S[%d]: %u mod %u  == %u|",dest_row,i,j,R_a,S_b,S_c);
				for(; k < R_cols; ++k){
					dest[dest_row *  dest_cols + k] = dR[i * R_cols + k];
					// printf(" % d",dR[i * R_cols + k]);
				}
				for(; l < S_cols; ++l){
					dest[dest_row *  dest_cols + k + l] = dS[j * S_cols + l];
					// printf(" % d",dS[j * S_cols + l]);
				}
				// LINE;
				dest_row++;
			}
			
		}
	}
	*dest_rows = dest_row;
}


void q3_index(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols){
	size_t dest_row = 0;
	size_t dest_col = 2;
	uint32_t R_a;
	uint32_t S_b;
	uint32_t S_c;
	for(size_t  i = 0; i < R_rows; i++){
		for(size_t j = 0 ; j < S_rows; j++){
			R_a = dR[i * R_cols + 0];
			S_b = dS[j * S_cols + 1];
			S_c = dS[j * S_cols + 2];
			if(S_b != 0 && R_a % S_b == S_c){
				dest[dest_row * dest_col] = i;
				dest[dest_row * dest_col  + 1] = j;
				dest_row++;
			}
		}
	}
	*dest_rows = dest_row;
}



// we assume for now that R and S have the same weaving parameters
void q3_weave(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows,uint32_t *R_buffer, uint32_t *S_buffer, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
	size_t cl_block_size = wordsize * cl_size;

	size_t dest_index = 0;
	size_t dest_cols = R_cols + S_cols;

	size_t R_cl_index;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_cl_index;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;
	
	for(size_t i = 0; i < R_rows; ++i){
		for(size_t m = 0; m < R_cols;++m){
			R_buffer[m] = 0;
		}
		size_t R_cl_block_index = (i * R_cols) / cl_block_size;
		size_t R_cl_index = (i % R_samples_per_cl) / R_samples_per_entry; 
		size_t R_shift_index = (i % R_samples_per_entry) * R_cols;
		
		for(size_t k = 0;k < wordsize; ++k){
			size_t R_i_index = R_cl_block_index * cl_block_size + k * cl_size + R_cl_index;

			for(size_t m = 0; m < R_cols;++m){
				uint32_t kth_bit_m = (dR[R_i_index] >> (R_shift_index + m) ) &1;
				R_buffer[m] += (kth_bit_m << (wordsize - k - 1));


			}


			// printf("[%d]block_index=%d,cl_index=%d,shift_index=%d => [%d]\n",i,R_cl_block_index,R_cl_index,R_shift_index,R_i_index);
	
		}
		// PRINT_MALLOC_H(R_buffer,R_cols);
		// LINE;
		// printf("[%d]block_index=%d,cl_index=%d,shift_index=%d\n",i,R_cl_block_index,R_cl_index,R_shift_index);
		for(size_t j = 0; j < S_rows;++j){
			for(size_t n = 0; n < S_cols;++n){
				S_buffer[n] = 0;
			}

			size_t S_cl_block_index = (j * S_cols) / cl_block_size;
			size_t S_cl_index = (j % S_samples_per_cl) / S_samples_per_entry; 
			size_t S_shift_index = (j % S_samples_per_entry) * S_cols;

			for(size_t l = 0; l < wordsize; ++l){

				size_t S_j_index = S_cl_block_index * cl_size * wordsize + l * cl_size + S_cl_index;
				for(size_t n = 0; n < S_cols;++n){
					uint32_t kth_bit_n = (dS[S_j_index] >> (S_shift_index + n) ) &1;
					S_buffer[n] += (kth_bit_n << (wordsize - l - 1));
				}
			}

			if(i == 0){
				// printf("%u %u %u %u \n",S_buffer[0],S_buffer[1],S_buffer[2],S_buffer[3]);
				// PRINT_MALLOC_H(S_buffer,S_cols);
				// LINE;
			}


			if(S_buffer[1] != 0 && (R_buffer[0]) % S_buffer[1] == S_buffer[2]){ // harcoded R.a, S.b,S.c
				size_t m = 0;
				size_t n = 0;
				for(; m < R_cols; ++m){
					dest[dest_index * dest_cols + m] = R_buffer[m];
			
				}
				
				for(; n < S_cols; ++n){
					dest[dest_index * dest_cols + m + n] = S_buffer[n];
					
				}
				dest_index++;
			}		
		}
	}
	*dest_rows = dest_index;
}


void q3_weave_index(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
	

	size_t cl_block_size = wordsize * cl_size;

	size_t dest_index = 0;

	size_t R_cl_index;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_cl_index;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;
	


	uint32_t R_a;
	uint32_t S_b;
	uint32_t S_c;

	

	// 512 * 32 = 2 ^ 16 -> L1 cache 
	for(size_t i = 0; i < R_rows; ++i){

		R_a = 0;
		size_t R_cl_block_index = (i * R_cols) / cl_block_size;
		size_t R_cl_index = (i % R_samples_per_cl) / R_samples_per_entry; 
		size_t R_shift_index = (i % R_samples_per_entry) * R_cols;
		


		for(size_t k = 0;k < wordsize; ++k){
			size_t R_i_index = R_cl_block_index * cl_block_size + k * cl_size + R_cl_index;
			uint32_t kth_bit_of_R_a = (dR[R_i_index] >> R_shift_index ) &1;
			R_a += kth_bit_of_R_a << (wordsize - k - 1);
	
		}
		for(size_t j = 0; j < S_rows;++j){
			S_b = 0;
			S_c = 0;
			size_t S_cl_block_index = (j * S_cols) / cl_block_size;
			size_t S_cl_index = (j % S_samples_per_cl) / S_samples_per_entry; 
			size_t S_shift_index = (j % S_samples_per_entry) * S_cols;

			for(size_t l = 0; l < wordsize; ++l){

				size_t S_j_index = S_cl_block_index * cl_size * wordsize + l * cl_size + S_cl_index;

				uint32_t lth_bit_of_S_b = (dS[S_j_index] >> (S_shift_index + 1)) & 1;
				uint32_t lth_bit_of_S_c = (dS[S_j_index] >> (S_shift_index + 2)) & 1;
				S_b += (lth_bit_of_S_b << (wordsize - l - 1));
				S_c += (lth_bit_of_S_c << (wordsize - l - 1));
			}

			if(S_b != 0 && R_a % S_b == S_c){
				dest[dest_index * 2] = i;
				dest[dest_index * 2 + 1] = j;
				dest_index++;
			}
		}
	}
	*dest_rows = dest_index;
}


void q3_weave_index_l1_block(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){


	size_t dest_index = 0;

	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;


	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));


	for(size_t i = 0; i < R_num_cl_blocks; ++i){
		memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
		for(size_t k  = 0; k < wordsize;++k){
			for(size_t m = 0; m < cl_size; ++m){
				uint32_t next_word = dR[i * cl_block_size + k * cl_size + m];
				for (size_t n = 0; n < R_samples_per_entry;++n){
					uint32_t k_th_bit_R_a = (next_word >> (n * R_cols)) & 1;
					R_a_buffer[m * R_samples_per_entry + n] += (k_th_bit_R_a << (wordsize - k - 1));
				}
			}
		}


		for(size_t j = 0; j < S_num_cl_blocks;++j){

			memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
			memset(S_c_buffer,0,S_smpls_per_cl_block * 4);
			for(size_t k  = 0 ; k < wordsize; ++k){
				for (size_t m = 0; m < cl_size; ++m){
					uint32_t next_word = dS[j * cl_block_size + k * cl_size + m];

					if(i == 0){

					}
					for (size_t n = 0; n < S_samples_per_entry;++n){
			
						uint32_t k_th_bit_S_b = (next_word >> (n * S_cols + 1)) & 1;
						uint32_t k_th_bit_S_c = (next_word >> (n * S_cols + 2)) & 1;

						// TODO:  HERE WE WOULD ADD THE BIT BY BIT IMPLEMENTATION
						S_b_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_b << (wordsize - k - 1));
						S_c_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_c << (wordsize - k - 1));
					}
				}
				
			}
			

			HLINE;
			// PRINT_MALLOC(R_a_buffer,R_smpls_per_cl_block,1);
			// PRINT_MALLOC(R_a_buffer,R_smpls_per_cl_block,1);
			PRINT_MALLOC(S_b_buffer,S_smpls_per_cl_block,1);
			// For now we do the actual join here:
			for(size_t k = 0; k < R_smpls_per_cl_block; ++k){
				for(size_t l = 0; l < S_smpls_per_cl_block; ++l){
					if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
						dest[2 * dest_index] = i * R_smpls_per_cl_block + k; 
						dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + l;
						dest_index++;
					} 
				}
			}

		}
	}
	
	*dest_rows = dest_index;
}



void q3_unroll_v2(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){


	size_t dest_index = 0;

	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;


	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));


	for(size_t i = 0; i < R_num_cl_blocks; ++i){
		memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
		for(size_t k  = 0; k < wordsize;++k){
			uint32_t word_shift_index = wordsize - k - 1;
			for(size_t m = 0; m < cl_size; m += 8){
				uint32_t next_word0 = dR[i * cl_block_size + k * cl_size + m + 0];
				uint32_t next_word1 = dR[i * cl_block_size + k * cl_size + m + 1];
				uint32_t next_word2 = dR[i * cl_block_size + k * cl_size + m + 2];
				uint32_t next_word3 = dR[i * cl_block_size + k * cl_size + m + 3];
				uint32_t next_word4 = dR[i * cl_block_size + k * cl_size + m + 4];
				uint32_t next_word5 = dR[i * cl_block_size + k * cl_size + m + 5];
				uint32_t next_word6 = dR[i * cl_block_size + k * cl_size + m + 6];
				uint32_t next_word7 = dR[i * cl_block_size + k * cl_size + m + 7];
				// printf("%u,%u,%u,%u,%u,%u,%u,%u\n",next_word0,next_word1,next_word2,next_word3,next_word4,next_word5,next_word6,next_word7);
				for (size_t n = 0; n < R_samples_per_entry;++n){
					uint32_t n_shift_index = n * R_cols;
					uint32_t k_th_bit_R_a0 = (next_word0 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a1 = (next_word1 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a2 = (next_word2 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a3 = (next_word3 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a4 = (next_word4 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a5 = (next_word5 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a6 = (next_word6 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a7 = (next_word7 >> n_shift_index) & 1;
					

					// printf("%u,%u,%u,%u,%u,%u,%u,%u\n",k_th_bit_R_a0,k_th_bit_R_a1,k_th_bit_R_a2,k_th_bit_R_a3,k_th_bit_R_a4,k_th_bit_R_a5,k_th_bit_R_a6,k_th_bit_R_a7);

					// (m + 0) * R_samples_per_entry + n -> (n * cl_size + (m+0))
					// R_a_buffer [(m + 0) * R_samples_per_entry + n] += (k_th_bit_R_a0 << word_shift_index);
					// R_a_buffer [(m + 1) * R_samples_per_entry + n] += (k_th_bit_R_a1 << word_shift_index);
					// R_a_buffer [(m + 2) * R_samples_per_entry + n] += (k_th_bit_R_a2 << word_shift_index);
					// R_a_buffer [(m + 3) * R_samples_per_entry + n] += (k_th_bit_R_a3 << word_shift_index);
					// R_a_buffer [(m + 4) * R_samples_per_entry + n] += (k_th_bit_R_a4 << word_shift_index);
					// R_a_buffer [(m + 5) * R_samples_per_entry + n] += (k_th_bit_R_a5 << word_shift_index);
					// R_a_buffer [(m + 6) * R_samples_per_entry + n] += (k_th_bit_R_a6 << word_shift_index);
					// R_a_buffer [(m + 7) * R_samples_per_entry + n] += (k_th_bit_R_a7 << word_shift_index);
					R_a_buffer [n * cl_size + m + 0] += (k_th_bit_R_a0 << word_shift_index);
					R_a_buffer [n * cl_size + m + 1] += (k_th_bit_R_a1 << word_shift_index);
					R_a_buffer [n * cl_size + m + 2] += (k_th_bit_R_a2 << word_shift_index);
					R_a_buffer [n * cl_size + m + 3] += (k_th_bit_R_a3 << word_shift_index);
					R_a_buffer [n * cl_size + m + 4] += (k_th_bit_R_a4 << word_shift_index);
					R_a_buffer [n * cl_size + m + 5] += (k_th_bit_R_a5 << word_shift_index);
					R_a_buffer [n * cl_size + m + 6] += (k_th_bit_R_a6 << word_shift_index);
					R_a_buffer [n * cl_size + m + 7] += (k_th_bit_R_a7 << word_shift_index);
					
					
				}
		
			}
		
		}

		// PRINT_MALLOC(R_a_buffer,R_smpls_per_cl_block,1);
		// return;
		// HLINE;


		for(size_t j = 0; j < S_num_cl_blocks;++j){

			memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
			memset(S_c_buffer,0,S_smpls_per_cl_block * 4);
			for(size_t k  = 0 ; k < wordsize; ++k){
				for (size_t m = 0; m < cl_size; ++m){
					uint32_t next_word = dS[j * cl_block_size + k * cl_size + m];

					if(i == 0){

					}
					for (size_t n = 0; n < S_samples_per_entry;++n){
			
						uint32_t k_th_bit_S_b = (next_word >> (n * S_cols + 1)) & 1;
						uint32_t k_th_bit_S_c = (next_word >> (n * S_cols + 2)) & 1;

						// TODO:  HERE WE WOULD ADD THE BIT BY BIT IMPLEMENTATION
						S_b_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_b << (wordsize - k - 1));
						S_c_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_c << (wordsize - k - 1));
					}
				}
				
			}
			

			// For now we do the actual join here:
			for(size_t k = 0; k < R_smpls_per_cl_block; ++k){
				for(size_t l = 0; l < S_smpls_per_cl_block; ++l){
					if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
						dest[2 * dest_index] = i * R_smpls_per_cl_block + k; 
						dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + l;
						dest_index++;
					} 
				}
			}

		}
	}
	
	*dest_rows = dest_index;
}



void q3_unroll(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){


	size_t dest_index = 0;

	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;


	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));


	for(size_t i = 0; i < R_num_cl_blocks; ++i){
		// memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
		uint32_t R_a_buffer0 = 0;
		uint32_t R_a_buffer1 = 0;
		uint32_t R_a_buffer2 = 0;
		uint32_t R_a_buffer3 = 0;
		uint32_t R_a_buffer4 = 0;
		uint32_t R_a_buffer5 = 0;
		uint32_t R_a_buffer6 = 0;
		uint32_t R_a_buffer7 = 0;

		for(size_t k  = 0; k < wordsize;++k){
			uint32_t word_shift_index = wordsize - k - 1;
			for(size_t m = 0; m < cl_size; m+= 8){ //unroll here


				uint32_t next_word0 = dR[i * cl_block_size + k * cl_size + m + 0];
				uint32_t next_word1 = dR[i * cl_block_size + k * cl_size + m + 1];
				uint32_t next_word2 = dR[i * cl_block_size + k * cl_size + m + 2];
				uint32_t next_word3 = dR[i * cl_block_size + k * cl_size + m + 3];
				uint32_t next_word4 = dR[i * cl_block_size + k * cl_size + m + 4];
				uint32_t next_word5 = dR[i * cl_block_size + k * cl_size + m + 5];
				uint32_t next_word6 = dR[i * cl_block_size + k * cl_size + m + 6];
				uint32_t next_word7 = dR[i * cl_block_size + k * cl_size + m + 7];

				for (size_t n = 0; n < R_samples_per_entry;++n){
					uint32_t n_shift_index = n * R_cols;

					uint32_t k_th_bit_R_a0 = (next_word0 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a1 = (next_word1 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a2 = (next_word2 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a3 = (next_word3 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a4 = (next_word4 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a5 = (next_word5 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a6 = (next_word6 >> n_shift_index) & 1;
					uint32_t k_th_bit_R_a7 = (next_word7 >> n_shift_index) & 1;

					R_a_buffer0 += (k_th_bit_R_a0 << word_shift_index);
					R_a_buffer1 += (k_th_bit_R_a1 << word_shift_index);
					R_a_buffer2 += (k_th_bit_R_a2 << word_shift_index);
					R_a_buffer3 += (k_th_bit_R_a3 << word_shift_index);
					R_a_buffer4 += (k_th_bit_R_a4 << word_shift_index);
					R_a_buffer5 += (k_th_bit_R_a5 << word_shift_index);
					R_a_buffer6 += (k_th_bit_R_a6 << word_shift_index);
					R_a_buffer7 += (k_th_bit_R_a7 << word_shift_index);
				}

				PRINT_MALLOC(R_a_buffer,8,1);
			}
		}


		for(size_t j = 0; j < S_num_cl_blocks;++j){



			uint32_t S_b_buffer0 = 0;
			uint32_t S_b_buffer1 = 0;
			uint32_t S_b_buffer2 = 0;
			uint32_t S_b_buffer3 = 0;
			uint32_t S_b_buffer4 = 0;
			uint32_t S_b_buffer5 = 0;
			uint32_t S_b_buffer6 = 0;
			uint32_t S_b_buffer7 = 0;

			uint32_t S_c_buffer0 = 0;
			uint32_t S_c_buffer1 = 0;
			uint32_t S_c_buffer2 = 0;
			uint32_t S_c_buffer3 = 0;
			uint32_t S_c_buffer4 = 0;
			uint32_t S_c_buffer5 = 0;
			uint32_t S_c_buffer6 = 0;
			uint32_t S_c_buffer7 = 0;

			// memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
			// memset(S_c_buffer,0,S_smpls_per_cl_block * 4);
			for(size_t k  = 0 ; k < wordsize; ++k){
				uint32_t word_shift_index = wordsize - k - 1;
				for (size_t m = 0; m < cl_size; ++m){
					// uint32_t next_word = dS[j * cl_block_size + k * cl_size + m];

					uint32_t next_word0 = dS[j * cl_block_size + k * cl_size + m + 0];
					uint32_t next_word1 = dS[j * cl_block_size + k * cl_size + m + 1];
					uint32_t next_word2 = dS[j * cl_block_size + k * cl_size + m + 2];
					uint32_t next_word3 = dS[j * cl_block_size + k * cl_size + m + 3];
					uint32_t next_word4 = dS[j * cl_block_size + k * cl_size + m + 4];
					uint32_t next_word5 = dS[j * cl_block_size + k * cl_size + m + 5];
					uint32_t next_word6 = dS[j * cl_block_size + k * cl_size + m + 6];
					uint32_t next_word7 = dS[j * cl_block_size + k * cl_size + m + 7];
					// if(i == 0){

					// }
					for (size_t n = 0; n < S_samples_per_entry;++n){
						uint32_t n_shift_b_index = n * R_cols + 1;
						uint32_t n_shift_c_index = n * R_cols + 2;
						uint32_t k_th_bit_S_b0 = (next_word0 >> n_shift_b_index) & 1;
						uint32_t k_th_bit_S_b1 = (next_word1 >> n_shift_b_index) & 1;
						uint32_t k_th_bit_S_b2 = (next_word2 >> n_shift_b_index) & 1;
						uint32_t k_th_bit_S_b3 = (next_word3 >> n_shift_b_index) & 1;
						uint32_t k_th_bit_S_b4 = (next_word4 >> n_shift_b_index) & 1;
						uint32_t k_th_bit_S_b5 = (next_word5 >> n_shift_b_index) & 1;
						uint32_t k_th_bit_S_b6 = (next_word6 >> n_shift_b_index) & 1;
						uint32_t k_th_bit_S_b7 = (next_word7 >> n_shift_b_index) & 1;



						uint32_t k_th_bit_S_c0 = (next_word0 >> n_shift_c_index) & 1;
						uint32_t k_th_bit_S_c1 = (next_word1 >> n_shift_c_index) & 1;
						uint32_t k_th_bit_S_c2 = (next_word2 >> n_shift_c_index) & 1;
						uint32_t k_th_bit_S_c3 = (next_word3 >> n_shift_c_index) & 1;
						uint32_t k_th_bit_S_c4 = (next_word4 >> n_shift_c_index) & 1;
						uint32_t k_th_bit_S_c5 = (next_word5 >> n_shift_c_index) & 1;
						uint32_t k_th_bit_S_c6 = (next_word6 >> n_shift_c_index) & 1;
						uint32_t k_th_bit_S_c7 = (next_word7 >> n_shift_c_index) & 1;




						S_b_buffer0 += (k_th_bit_S_b0 << word_shift_index);
						S_b_buffer1 += (k_th_bit_S_b1 << word_shift_index);
						S_b_buffer2 += (k_th_bit_S_b2 << word_shift_index);
						S_b_buffer3 += (k_th_bit_S_b3 << word_shift_index);
						S_b_buffer4 += (k_th_bit_S_b4 << word_shift_index);
						S_b_buffer5 += (k_th_bit_S_b5 << word_shift_index);
						S_b_buffer6 += (k_th_bit_S_b6 << word_shift_index);
						S_b_buffer7 += (k_th_bit_S_b7 << word_shift_index);
						
						S_c_buffer0 += (k_th_bit_S_c0 << word_shift_index);
						S_c_buffer1 += (k_th_bit_S_c1 << word_shift_index);
						S_c_buffer2 += (k_th_bit_S_c2 << word_shift_index);
						S_c_buffer3 += (k_th_bit_S_c3 << word_shift_index);
						S_c_buffer4 += (k_th_bit_S_c4 << word_shift_index);
						S_c_buffer5 += (k_th_bit_S_c5 << word_shift_index);
						S_c_buffer6 += (k_th_bit_S_c6 << word_shift_index);
						S_c_buffer7 += (k_th_bit_S_c7 << word_shift_index);


						// // uint32_t k_th_bit_S_c = (next_word >> n_shift_c_index) & 1

						// // TODO:  HERE WE WOULD ADD THE BIT BY BIT IMPLEMENTATION
						// S_b_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_b << word_shift_index);
						// S_c_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_c << word_shift_index);
					}
				}
				
			}
			

		// if(S_b_buffer0 != 0 && R_a_buffer0 % S_b_buffer0 == S_c_buffer0){
		// 	dest[2 * dest_index] = i * R_smpls_per_cl_block;
		// }



		// For now we do the actual join here:
		// for(size_t k = 0; k < R_smpls_per_cl_block; ++k){
		// 	for(size_t l = 0; l < S_smpls_per_cl_block; ++l){
		// 		if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
		// 			dest[2 * dest_index] = i * R_smpls_per_cl_block + k; 
		// 			dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + l;
		// 			dest_index++;
		// 		} 
		// 	}
		}
	}
	
	*dest_rows = dest_index;
}


void q3_vector(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){


	size_t dest_index = 0;

	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;


	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));


	__m256i R_a_vector;
	__m256i next_word;
	for(size_t i = 0; i < R_num_cl_blocks; ++i){
		
		memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
		for(size_t k  = 0; k < wordsize;++k){
			uint32_t word_shift_index = wordsize - k - 1;
			for(size_t m = 0; m < cl_size; m+= 8){ //unroll here


				//here we can do better and get rid of index calcs
				// printf("Index : 0x%lx\n",(uint64_t) (dR + i * cl_block_size + k * cl_size + m) % 32);
				next_word = _mm256_loadu_si256(dR + i * cl_block_size + k * cl_size + m);
				// PRINT_32_BIT_VECTOR(next_word);
				for (size_t n = 0; n < R_samples_per_entry;++n){
					uint32_t n_shift_index = n * R_cols;
					// her we can get rid of a shift I think
					__m256i k_th_bit_R_a = _mm256_srli_epi32(next_word,n_shift_index);
					k_th_bit_R_a = _mm256_and_si256(k_th_bit_R_a,_mm256_set_epi32(1,1,1,1,1,1,1,1)); 
					k_th_bit_R_a = _mm256_slli_epi32(k_th_bit_R_a,word_shift_index);
					// PRINT_32_BIT_VECTOR(k_th_bit_R_a);
					R_a_vector = _mm256_loadu_si256(R_a_buffer + n * cl_size + m);
					R_a_vector = _mm256_add_epi32(R_a_vector,k_th_bit_R_a);
					_mm256_store_si256(R_a_buffer + n * cl_size + m,R_a_vector);
				}
			}



		}

		// PRINT_MALLOC(R_a_buffer,R_smpls_per_cl_block,1);

		__m256i S_b_vector;
		__m256i S_c_vector;
		for(size_t j = 0; j < S_num_cl_blocks;++j){
			
			memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
			memset(S_c_buffer,0,S_smpls_per_cl_block * 4);

			for(size_t k  = 0 ; k < wordsize; ++k){
				uint32_t word_shift_index = wordsize - k - 1;
				for (size_t m = 0; m < cl_size; m += 8){


					next_word = _mm256_loadu_si256(dS + j * cl_block_size + k * cl_size + m);
				
					for (size_t n = 0; n < S_samples_per_entry;++n){
						uint32_t n_shift_b_index = n * S_cols + 1;
						uint32_t n_shift_c_index = n * S_cols + 2;

						

						__m256i k_th_bit_S_b = _mm256_srli_epi32(next_word,n_shift_b_index);
						k_th_bit_S_b = _mm256_and_si256(k_th_bit_S_b,_mm256_set_epi32(1,1,1,1,1,1,1,1));
						k_th_bit_S_b = _mm256_slli_epi32(k_th_bit_S_b,word_shift_index);
						S_b_vector = _mm256_add_epi32(S_b_vector,k_th_bit_S_b);
						S_b_vector = _mm256_loadu_si256(S_b_buffer + n * cl_size + m);
						S_b_vector = _mm256_add_epi32(S_b_vector,k_th_bit_S_b);
						_mm256_storeu_si256(S_b_buffer + n * cl_size + m,S_b_vector);

						
						__m256i k_th_bit_S_c = _mm256_srli_epi32(next_word,n_shift_c_index);
						k_th_bit_S_c = _mm256_and_si256(k_th_bit_S_c,_mm256_set_epi32(1,1,1,1,1,1,1,1));
						k_th_bit_S_c = _mm256_slli_epi32(k_th_bit_S_c,word_shift_index);
						S_c_vector = _mm256_add_epi32(S_c_vector,k_th_bit_S_c);
						S_c_vector = _mm256_loadu_si256(S_c_buffer + n * cl_size + m);
						S_c_vector = _mm256_add_epi32(S_c_vector,k_th_bit_S_c);
						_mm256_storeu_si256(S_c_buffer + n * cl_size + m,S_c_vector);
					}
				}
				
			}

			// HLINE;
			// // PRINT_MALLOC(R_a_buffer,R_smpls_per_cl_block,1);
			// HLINE;
			// PRINT_MALLOC(S_b_buffer,S_smpls_per_cl_block,1);
			// PRINT_MALLOC()
			// For now we do the actual join here:
			for(size_t k = 0; k < R_smpls_per_cl_block; ++k){

				for(size_t l = 0; l < S_smpls_per_cl_block; ++l){
					if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
						size_t r_i = (k % cl_size) * R_samples_per_entry + (k / cl_size);
						size_t s_j = (l % cl_size) * S_samples_per_entry + (l / cl_size);
						// printf("%d,%d\n",i * R_smpls_per_cl_block + r_i,j * S_smpls_per_cl_block + s_j); 
						// printf("%d,%d,%d\n",R_a_buffer[k],S_b_buffer[l],j * S_smpls_per_cl_block + s_j,S_c_buffer[l]); 
						dest[2 * dest_index] = i * R_smpls_per_cl_block + r_i;
						dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + s_j;
						dest_index++;
					} 
				}
			}
		}

	}
	// HLINE;
	*dest_rows = dest_index;
	// PRINT_MALLOC(dest,*dest_rows,2);
}
















		// 	// For now we do the actual join here:
		// 	for(size_t k = 0; k < R_smpls_per_cl_block; k += 1){
		// 		uint32_t R_a_k0 = R_a_buffer[k + 0];
		// 		// uint32_t R_a_k1 = R_a_buffer[k + 1];
		// 		// uint32_t R_a_k2 = R_a_buffer[k + 2];
		// 		// uint32_t R_a_k3 = R_a_buffer[k + 3];
		// 		// uint32_t R_a_k4 = R_a_buffer[k + 4];
		// 		// uint32_t R_a_k5 = R_a_buffer[k + 5];
		// 		// uint32_t R_a_k6 = R_a_buffer[k + 6];
		// 		// uint32_t R_a_k7 = R_a_buffer[k + 7];


		// 		for(size_t l = 0; l < S_smpls_per_cl_block; l += 8){
		// 			uint32_t S_b_l0 = S_b_buffer[l + 0];
		// 			uint32_t S_b_l1 = S_b_buffer[l + 1];
		// 			uint32_t S_b_l2 = S_b_buffer[l + 2];
		// 			uint32_t S_b_l3 = S_b_buffer[l + 3];
		// 			uint32_t S_b_l4 = S_b_buffer[l + 4];
		// 			uint32_t S_b_l5 = S_b_buffer[l + 5];
		// 			uint32_t S_b_l6 = S_b_buffer[l + 6];
		// 			uint32_t S_b_l7 = S_b_buffer[l + 7];



		// 			uint32_t S_c_l0 = S_c_buffer[l + 0];
		// 			uint32_t S_c_l1 = S_c_buffer[l + 1];
		// 			uint32_t S_c_l2 = S_c_buffer[l + 2];
		// 			uint32_t S_c_l3 = S_c_buffer[l + 3];
		// 			uint32_t S_c_l4 = S_c_buffer[l + 4];
		// 			uint32_t S_c_l5 = S_c_buffer[l + 5];
		// 			uint32_t S_c_l6 = S_c_buffer[l + 6];
		// 			uint32_t S_c_l7 = S_c_buffer[l + 7];
				


		// 		}
		// 	}

		// }




//SPECIAL CASES:
// S_b <= S_c  -> discard
// R_a <= S_b -> equality check
// 
void q3_prune(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){




	size_t dest_index = 0;

	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;


	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));



	
	// here we only need 1 bit for each entry, keeping it at uint32_t for now
	uint32_t * b_le_c_buffer = (uint32_t*) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * b_le_c_buffer_helper = (uint32_t*) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));

	uint32_t xor_b_c;

	for(size_t i = 0; i < R_num_cl_blocks; ++i){
		memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
		for(size_t k  = 0; k < wordsize;++k){
			for(size_t m = 0; m < cl_size; ++m){
				uint32_t next_word = dR[i * cl_block_size + k * cl_size + m];
				for (size_t n = 0; n < R_samples_per_entry;++n){
					uint32_t k_th_bit_R_a = (next_word >> (n * R_cols)) & 1;
					R_a_buffer[m * R_samples_per_entry + n] += (k_th_bit_R_a << (wordsize - k - 1));
				}
			}
		}


		for(size_t j = 0; j < S_num_cl_blocks;++j){

			memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
			memset(S_c_buffer,0,S_smpls_per_cl_block * 4);
			memset(b_le_c_buffer,0,S_smpls_per_cl_block * 4);
			memset(b_le_c_buffer_helper,0,S_smpls_per_cl_block * 4);
			for(size_t k  = 0 ; k < wordsize; ++k){
				for (size_t m = 0; m < cl_size; ++m){
					uint32_t next_word = dS[j * cl_block_size + k * cl_size + m];
					for (size_t n = 0; n < S_samples_per_entry;++n){
						

						
						uint32_t k_th_bit_S_b = (next_word >> (n * S_cols + 1)) & 1;
						uint32_t k_th_bit_S_c = (next_word >> (n * S_cols + 2)) & 1;
						uint32_t xor_b_c = k_th_bit_S_b ^ k_th_bit_S_c;
						uint32_t xorb = xor_b_c & k_th_bit_S_b;
						uint32_t xorc = xor_b_c & k_th_bit_S_c;
						// TODO:  HERE WE WOULD ADD THE BIT BY BIT IMPLEMENTATION
						S_b_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_b << (wordsize - k - 1));
						S_c_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_c << (wordsize - k - 1));
					}
				}
				
			}
			

			// For now we do the actual join here:
			for(size_t k = 0; k < R_smpls_per_cl_block; ++k){
				for(size_t l = 0; l < S_smpls_per_cl_block; ++l){
					if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
						dest[2 * dest_index] = i * R_smpls_per_cl_block + k; 
						dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + l;
						dest_index++;
					} 
				}
			}

		}
	}
	*dest_rows = dest_index;
}








void q3_weave_index_l2_block(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size,size_t block_size){



	size_t dest_index = 0;

	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_num_cl_blocks_blocks = R_num_cl_blocks / block_size;
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_num_cl_blocks_blocks = S_num_cl_blocks / block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;


	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));



	for (size_t b_i = 0; b_i < R_num_cl_blocks_blocks;b_i++){
		for(size_t i = 0; i < block_size; ++i){
			memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
			for(size_t k  = 0; k < wordsize;++k){
				for(size_t m = 0; m < cl_size; ++m){
					uint32_t next_word = dR[(b_i * block_size + i) * cl_block_size + k * cl_size + m];
					for (size_t n = 0; n < R_samples_per_entry;++n){
						uint32_t k_th_bit_R_a = (next_word >> (n * R_cols)) & 1;
						R_a_buffer[m * R_samples_per_entry + n] += (k_th_bit_R_a << (wordsize - k - 1));
					}
				}
			}


			for (size_t b_j = 0; b_j < S_num_cl_blocks_blocks;b_j++){
				for(size_t j = 0; j < block_size;++j){

					memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
					memset(S_c_buffer,0,S_smpls_per_cl_block * 4);
					for(size_t k  = 0 ; k < wordsize; ++k){
						for (size_t m = 0; m < cl_size; ++m){
							uint32_t next_word = dS[(b_j * block_size + j) * cl_block_size + k * cl_size + m];

							if(i == 0){

							}
							for (size_t n = 0; n < S_samples_per_entry;++n){
					
								uint32_t k_th_bit_S_b = (next_word >> (n * S_cols + 1)) & 1;
								uint32_t k_th_bit_S_c = (next_word >> (n * S_cols + 2)) & 1;

								// TODO:  HERE WE WOULD ADD THE BIT BY BIT IMPLEMENTATION
								S_b_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_b << (wordsize - k - 1));
								S_c_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_c << (wordsize - k - 1));
							}
						}
						
					}
					

					// For now we do the actual join here:
					for(size_t k = 0; k < R_smpls_per_cl_block; ++k){
						for(size_t l = 0; l < S_smpls_per_cl_block; ++l){
							if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
								dest[2 * dest_index] = (b_i * block_size + i) * R_smpls_per_cl_block + k; 
								dest[2 * dest_index + 1] = (b_j * block_size + j) * S_smpls_per_cl_block + l;
								dest_index++;
							} 
						}
					}

				}
			}

		}
	}
	*dest_rows = dest_index;
}
