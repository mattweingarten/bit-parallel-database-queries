
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query3.h"
// #include "../include/tsc_x86.h"

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


	// int64_t start1,end1;
	// int64_t start2,end2;
	// int64_t start3,end3;
	uint64_t start,end;
	double cycles = 0;
	int msr_count =  0;

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
		// start1 = start_tsc();
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

		// end1 = stop_tsc(start1);

		
		for(size_t j = 0; j < S_num_cl_blocks;++j){

			start = start_tsc();

			// start2 = start_tsc();
			memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
			memset(S_c_buffer,0,S_smpls_per_cl_block * 4);
			for(size_t k  = 0 ; k < wordsize; ++k){
				for (size_t m = 0; m < cl_size; ++m){
					uint32_t next_word = dS[j * cl_block_size + k * cl_size + m];
					// printf("index : %d\n",i * cl_block_size + k * cl_size + m);
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

			end = stop_tsc( start);
			cycles += (double) end;

			msr_count++;

			

			// end2 = stop_tsc(start2);

			// HLINE;
			// PRINT_MALLOC(R_a_buffer,R_smpls_per_cl_block,1);
			// PRINT_MALLOC(R_a_buffer,R_smpls_per_cl_block,1);
			// PRINT_MALLOC(S_b_buffer,S_smpls_per_cl_block,1);
			// For now we do the actual join here:

			// start3 = start_tsc();
			for(size_t k = 0; k < R_smpls_per_cl_block; ++k){
				for(size_t l = 0; l < S_smpls_per_cl_block; ++l){
					if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
						dest[2 * dest_index] = i * R_smpls_per_cl_block + k; 
						dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + l;
						dest_index++;
					} 
				}
			}
			// end3 = stop_tsc(start3);
			// printf("%u %u %u\n",end1,end2,end3);
		}
	}
	
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	*dest_rows = dest_index;
	printf("Cycles : %lf\n",cycles/msr_count);
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
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
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
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
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

	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
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

	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
}




void q3_vector_v2(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){


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
			for(size_t m = 0; m < cl_size; m+= 8){ 


				next_word = _mm256_loadu_si256(dR + i * cl_block_size + k * cl_size + m);
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



			for(size_t k = 0; k < R_smpls_per_cl_block; k+= 8){
				
				__m256i k_v = _mm256_loadu_si256(R_a_buffer + k);
				__m256 k_v_d = _mm256_cvtepi32_ps(k_v);



				for(size_t l = 0; l < S_smpls_per_cl_block; ++l){

					// if(S_b_buffer[l] != 0){

					// }
					// if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
					// 	size_t r_i = (k % cl_size) * R_samples_per_entry + (k / cl_size);
					// 	size_t s_j = (l % cl_size) * S_samples_per_entry + (l / cl_size);
	
					// 	dest[2 * dest_index] = i * R_smpls_per_cl_block + r_i;
					// 	dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + s_j;
					// 	dest_index++;
					// } 
				}
			}
		}

	}
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	*dest_rows = dest_index;
}




void q3_vector_v3(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){

	// int64_t start0,end0;
	// int64_t start1,end1;
	// int64_t start2,end2;
	// int64_t start3,end3;

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
		

		// start1 = start_tsc();


		memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
		for(size_t k  = 0; k < wordsize;++k){
			uint32_t word_shift_index = wordsize - k - 1;
			for(size_t m = 0; m < cl_size; m+= 8){ 


				next_word = _mm256_loadu_si256(dR + i * cl_block_size + k * cl_size + m);
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
		// end1 = stop_tsc(start1);


		__m256i S_b_vector;
		__m256i S_c_vector;
		for(size_t j = 0; j < S_num_cl_blocks;++j){
			

			// start2 = start_tsc();

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

			// end2 = stop_tsc(start2);

	

			// start3 = start_tsc();
			for(size_t l = 0; l < S_smpls_per_cl_block; ++l){
				uint32_t d = S_b_buffer[l];
				uint32_t s_c = S_c_buffer[l];
				uint32_t L,L2,m,sh1,sh2;

				switch(d){
					case 0: 
						
						continue; // here we just keep going and do not compute the values
						break;
					case 1:
						m = 1; sh1 = sh2 = 0;
						break;
					case 2:  
						m = 1; sh1 = 1; sh2 = 0;
						break;
					default:
						__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );  		//ceil(log2)
						L += 1;												//ceil(log2)
						L2 = (uint32_t) (L < 32? 1 << L : 0);
						m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
						sh1 = 1; sh2 = L-1;
				}


				__m256i m_v = _mm256_set1_epi32(m);

				__m256i d_v = _mm256_set1_epi32(d);
				__m256i s_c_v = _mm256_set1_epi32(s_c);


				for(size_t k = 0; k < R_smpls_per_cl_block; k+= 8){
				
					__m256i x_v = _mm256_loadu_si256(R_a_buffer + k);
					__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
					t_lo = _mm256_srli_epi64(t_lo,32);
					__m256i t_hi = _mm256_srli_epi64(x_v,32);
					t_hi = _mm256_mul_epu32(t_hi,m_v);
					__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
					__m256i res = _mm256_sub_epi32(x_v,t);
					res = _mm256_srli_epi32(res,sh1);
					res = _mm256_add_epi32(res,t);
					res = _mm256_srli_epi32(res,sh2);
					res = _mm256_mullo_epi32(res,d_v);
					res = _mm256_sub_epi32(x_v,res);
					__m256i eq = _mm256_cmpeq_epi32(res,s_c_v);
					uint32_t eq_vector[8];
					memcpy(eq_vector, &eq, sizeof(eq_vector));
	
					for(size_t h = 0; h < 8; ++h){
						// if(S_b_buffer[l] != 0 && R_a_buffer[k + h] % S_b_buffer[l] == S_c_buffer[l] && eq_vector[h] != 4294967295L){
						// 	printf("[%d] %u,%u,%u\n",h,R_a_buffer[k + h],S_b_buffer[l],S_c_buffer[l] );

						// 	// LINE;
						// 	PRINT_32_BIT_VECTOR(res);
						// 	PRINT_32_BIT_VECTOR(s_c_v);
						// 	PRINT_32_BIT_VECTOR(eq);
						// 	// printf("[%d]",eq)
						// 	LINE;		
						// }
						if(eq_vector[h] == 4294967295L){
							
							size_t r_i = ((h + k) % cl_size) * R_samples_per_entry + ((h + k) / cl_size);
							size_t s_j = (l % cl_size) * S_samples_per_entry + (l / cl_size);
							dest[2 * dest_index] = i * R_smpls_per_cl_block + r_i;
							dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + s_j;
							dest_index++;

						}
					}
				}
			}

			// end3 = stop_tsc(start3);

			// printf("%u %u %u\n",end1,end2,end3);
		}

	}
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	*dest_rows = dest_index;
}





void q3_vector_v4(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){


	// int64_t start1,end1;
	// int64_t start2,end2;
	// int64_t start3,end3;


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

	struct modular_operation* mod_ops  = (struct modular_operation*) aligned_alloc(32,S_smpls_per_cl_block * sizeof(struct modular_operation));

	__m256i S_b_vector;
	__m256i S_c_vector;
	__m256i next_word;
	for(size_t j = 0; j < S_num_cl_blocks;++j){
		// start1 = start_tsc();
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
		// Loaded and rebuilt 1 cl_block of S
		// Now we can compute the values for fast modulo, the cost of this is amortized since we only do this 
		// ONCE for each row in S
		//TODO can we vectorize this part? how much can we gain from this?
		for(size_t k = 0; k < S_smpls_per_cl_block;++k){ 
			uint32_t L,L2;
			uint32_t d = S_b_buffer[k];
			switch(d){
				case 0: 
					
					continue; // here we just keep going and do not compute the values
					break;
				case 1:
					mod_ops[k].m = 1; mod_ops[k].sh1 = mod_ops[k].sh2 = 0;
					break;
				case 2:  
					mod_ops[k].m = 1; mod_ops[k].sh1 = 1; mod_ops[k].sh2 = 0;
					break;
				default:
					__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );  		//ceil(log2)
					L += 1;												//ceil(log2)
					L2 = (uint32_t) (L < 32? 1 << L : 0);
					mod_ops[k].m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
					mod_ops[k].sh1 = 1; mod_ops[k].sh2 = L-1;
				}

		}

		// end1 = stop_tsc(start1);





		
		for(size_t i = 0; i < R_num_cl_blocks; ++i){
		__m256i R_a_vector;
		__m256i next_word_R;
		// start2 = start_tsc();
		memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
			for(size_t k  = 0; k < wordsize;++k){
				uint32_t word_shift_index = wordsize - k - 1;
				for(size_t m = 0; m < cl_size; m+= 8){ 


					next_word_R = _mm256_loadu_si256(dR + i * cl_block_size + k * cl_size + m);
					for (size_t n = 0; n < R_samples_per_entry;++n){
						uint32_t n_shift_index = n * R_cols;
						// her we can get rid of a shift I think
						__m256i k_th_bit_R_a = _mm256_srli_epi32(next_word_R,n_shift_index);
						k_th_bit_R_a = _mm256_and_si256(k_th_bit_R_a,_mm256_set_epi32(1,1,1,1,1,1,1,1)); 
						k_th_bit_R_a = _mm256_slli_epi32(k_th_bit_R_a,word_shift_index);
						// PRINT_32_BIT_VECTOR(k_th_bit_R_a);
						R_a_vector = _mm256_loadu_si256(R_a_buffer + n * cl_size + m);
						R_a_vector = _mm256_add_epi32(R_a_vector,k_th_bit_R_a);
						_mm256_store_si256(R_a_buffer + n * cl_size + m,R_a_vector);
					}
				}
			}

			// end2 = stop_tsc(start2);



			// start3 = start_tsc();
			for(size_t l = 0; l < S_smpls_per_cl_block;++l){
				struct modular_operation mod_l = mod_ops[l];
				uint32_t s_b = S_b_buffer[l];
				uint32_t s_c = S_c_buffer[l];


				__m256i m_v = _mm256_set1_epi32(mod_l.m);
				__m256i d_v = _mm256_set1_epi32(s_b);
				__m256i s_c_v = _mm256_set1_epi32(s_c);


				for(size_t k = 0; k < R_smpls_per_cl_block; k+= 8){
					__m256i x_v = _mm256_loadu_si256(R_a_buffer + k);
					__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
					t_lo = _mm256_srli_epi64(t_lo,32);
					__m256i t_hi = _mm256_srli_epi64(x_v,32);
					t_hi = _mm256_mul_epu32(t_hi,m_v);
					__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
					__m256i res = _mm256_sub_epi32(x_v,t);
					res = _mm256_srli_epi32(res,mod_l.sh1);
					res = _mm256_add_epi32(res,t);
					res = _mm256_srli_epi32(res,mod_l.sh2);
					res = _mm256_mullo_epi32(res,d_v);
					res = _mm256_sub_epi32(x_v,res);
					__m256i eq = _mm256_cmpeq_epi32(res,s_c_v);
					uint32_t eq_vector[8];
					memcpy(eq_vector, &eq, sizeof(eq_vector));

					// _mm256_testc_si256()

					for(size_t h = 0; h < 8; ++h){
						if(eq_vector[h] == 4294967295L){
							size_t r_i = ((h + k) % cl_size) * R_samples_per_entry + ((h + k) / cl_size);
							size_t s_j = (l % cl_size) * S_samples_per_entry + (l / cl_size);
							dest[2 * dest_index] = i * R_smpls_per_cl_block + r_i;
							dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + s_j;
							dest_index++;
						}
					}

				}

			}
			// end3 = stop_tsc(start3);
			// printf("%u %u %u\n",end1,end2,end3);
		}// end cl_block R
	} //end cl_block S



	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	free(mod_ops);
	*dest_rows = dest_index;
}


void q3_vector_v5(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){



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

	struct modular_operation* mod_ops  = (struct modular_operation*) aligned_alloc(32,S_smpls_per_cl_block * sizeof(struct modular_operation));


	uint32_t * S_next_word_index = dS;

	__m256i S_b_vector;
	__m256i S_c_vector;
	__m256i next_word;
	
	for(size_t j = 0; j < S_num_cl_blocks;++j){
		memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
		memset(S_c_buffer,0,S_smpls_per_cl_block * 4);

		uint32_t word_shift_index = wordsize  - 1;
		for(size_t k  = 0 ; k < wordsize; ++k){


			for (size_t m = 0; m < cl_size; m += 8){


				next_word = _mm256_loadu_si256(S_next_word_index);
				uint32_t n_shift_b_index = 1;
				uint32_t n_shift_c_index = 2;
				uint32_t* S_b_buffer_index = S_b_buffer + m;
				uint32_t* S_c_buffer_index = S_c_buffer + m;
				for (size_t n = 0; n < S_samples_per_entry;++n){
	
					__m256i k_th_bit_S_b = _mm256_srli_epi32(next_word,n_shift_b_index);
					k_th_bit_S_b = _mm256_and_si256(k_th_bit_S_b,_mm256_set_epi32(1,1,1,1,1,1,1,1));
					k_th_bit_S_b = _mm256_slli_epi32(k_th_bit_S_b,word_shift_index);
					S_b_vector = _mm256_add_epi32(S_b_vector,k_th_bit_S_b);
					S_b_vector = _mm256_loadu_si256(S_b_buffer_index);
					S_b_vector = _mm256_add_epi32(S_b_vector,k_th_bit_S_b);
					_mm256_storeu_si256(S_b_buffer_index,S_b_vector);

	
					__m256i k_th_bit_S_c = _mm256_srli_epi32(next_word,n_shift_c_index);
					k_th_bit_S_c = _mm256_and_si256(k_th_bit_S_c,_mm256_set_epi32(1,1,1,1,1,1,1,1));
					k_th_bit_S_c = _mm256_slli_epi32(k_th_bit_S_c,word_shift_index);
					S_c_vector = _mm256_add_epi32(S_c_vector,k_th_bit_S_c);
					S_c_vector = _mm256_loadu_si256(S_c_buffer_index);
					S_c_vector = _mm256_add_epi32(S_c_vector,k_th_bit_S_c);
					_mm256_storeu_si256(S_c_buffer_index,S_c_vector);


					n_shift_b_index += S_cols;
					n_shift_c_index += S_cols;
					S_b_buffer_index += cl_size;
					S_c_buffer_index += cl_size;
					// printf("Sc buffer index : %u\n",S_b_buffer_index - S_b_buffer);
				}
				S_next_word_index += 8;
			}
			word_shift_index -= 1;
		}

		// PRINT_MALLOC(S_b_buffer,S_smpls_per_cl_block,1);LINE;
		// Loaded and rebuilt 1 cl_block of S
		// Now we can compute the values for fast modulo, the cost of this is amortized since we only do this 
		// ONCE for each row in S
		//TODO can we vectorize this part? how much can we gain from this?
		for(size_t k = 0; k < S_smpls_per_cl_block;++k){ 
			uint32_t L,L2;
			uint32_t d = S_b_buffer[k];
			switch(d){
				case 0: 
					
					continue; // here we just keep going and do not compute the values
					break;
				case 1:
					mod_ops[k].m = 1; mod_ops[k].sh1 = mod_ops[k].sh2 = 0;
					break;
				case 2:  
					mod_ops[k].m = 1; mod_ops[k].sh1 = 1; mod_ops[k].sh2 = 0;
					break;
				default:
					__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );  		//ceil(log2)
					L += 1;												//ceil(log2)
					L2 = (uint32_t) (L < 32? 1 << L : 0);
					mod_ops[k].m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
					mod_ops[k].sh1 = 1; mod_ops[k].sh2 = L-1;
				}

		}






		uint32_t * R_next_word_index = dR;
		for(size_t i = 0; i < R_num_cl_blocks; ++i){
		__m256i R_a_vector;
		__m256i next_word_R;
		memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
			uint32_t word_shift_index = wordsize  - 1;
			for(size_t k  = 0; k < wordsize;++k){
				for(size_t m = 0; m < cl_size; m+= 8){ 

					uint32_t* R_a_buffer_index = R_a_buffer + m;

					next_word_R = _mm256_loadu_si256(R_next_word_index);
					uint32_t n_shift_index = 0;
					for (size_t n = 0; n < R_samples_per_entry;++n){
						__m256i k_th_bit_R_a = _mm256_srli_epi32(next_word_R,n_shift_index);
						k_th_bit_R_a = _mm256_and_si256(k_th_bit_R_a,_mm256_set_epi32(1,1,1,1,1,1,1,1)); 
						k_th_bit_R_a = _mm256_slli_epi32(k_th_bit_R_a,word_shift_index);
						R_a_vector = _mm256_loadu_si256(R_a_buffer_index);
						R_a_vector = _mm256_add_epi32(R_a_vector,k_th_bit_R_a);
						_mm256_store_si256(R_a_buffer_index,R_a_vector);
						

						n_shift_index += R_cols;
						R_a_buffer_index += cl_size;
					}
					R_next_word_index += 8;
				}
				word_shift_index -= 1;
			}


			for(size_t l = 0; l < S_smpls_per_cl_block;++l){
				struct modular_operation mod_l = mod_ops[l];
				uint32_t s_b = S_b_buffer[l];
				uint32_t s_c = S_c_buffer[l];


				__m256i m_v = _mm256_set1_epi32(mod_l.m);
				__m256i d_v = _mm256_set1_epi32(s_b);
				__m256i s_c_v = _mm256_set1_epi32(s_c);


				for(size_t k = 0; k < R_smpls_per_cl_block; k+= 8){
					__m256i x_v = _mm256_loadu_si256(R_a_buffer + k);
					__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
					t_lo = _mm256_srli_epi64(t_lo,32);
					__m256i t_hi = _mm256_srli_epi64(x_v,32);
					t_hi = _mm256_mul_epu32(t_hi,m_v);
					__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
					__m256i res = _mm256_sub_epi32(x_v,t);
					res = _mm256_srli_epi32(res,mod_l.sh1);
					res = _mm256_add_epi32(res,t);
					res = _mm256_srli_epi32(res,mod_l.sh2);
					res = _mm256_mullo_epi32(res,d_v);
					res = _mm256_sub_epi32(x_v,res);
					__m256i eq = _mm256_cmpeq_epi32(res,s_c_v);
					uint32_t* eq_vector = &eq;
		
					for(size_t h = 0; h < 8; ++h){
						if(eq_vector[h] == 4294967295L){
							size_t r_i = ((h + k) % cl_size) * R_samples_per_entry + ((h + k) / cl_size);
							size_t s_j = (l % cl_size) * S_samples_per_entry + (l / cl_size);
							dest[2 * dest_index] = i * R_smpls_per_cl_block + r_i;
							dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + s_j;
							dest_index++;
						}
					}
				}

			}
		}// end cl_block R

	} //end cl_block S



	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	free(mod_ops);
	*dest_rows = dest_index;
}




void q3_vector_v6(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){



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

	struct modular_operation* mod_ops  = (struct modular_operation*) aligned_alloc(32,S_smpls_per_cl_block * sizeof(struct modular_operation));



	// ========== For

	//


	uint32_t * S_next_word_index = dS;

	__m256i S_b_vector;
	__m256i S_c_vector;
	__m256i next_word;
	
	for(size_t j = 0; j < S_num_cl_blocks;++j){
		memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
		memset(S_c_buffer,0,S_smpls_per_cl_block * 4);

		uint32_t word_shift_index = wordsize  - 1;
		for(size_t k  = 0 ; k < wordsize; ++k){


			for (size_t m = 0; m < cl_size; m += 8){


				next_word = _mm256_loadu_si256(S_next_word_index);
				uint32_t n_shift_b_index = 1;
				uint32_t n_shift_c_index = 2;
				uint32_t* S_b_buffer_index = S_b_buffer + m;
				uint32_t* S_c_buffer_index = S_c_buffer + m;
				for (size_t n = 0; n < S_samples_per_entry;++n){
	
					__m256i k_th_bit_S_b = _mm256_srli_epi32(next_word,n_shift_b_index);
					k_th_bit_S_b = _mm256_and_si256(k_th_bit_S_b,_mm256_set_epi32(1,1,1,1,1,1,1,1));
					k_th_bit_S_b = _mm256_slli_epi32(k_th_bit_S_b,word_shift_index);
					S_b_vector = _mm256_add_epi32(S_b_vector,k_th_bit_S_b);
					S_b_vector = _mm256_loadu_si256(S_b_buffer_index);
					S_b_vector = _mm256_add_epi32(S_b_vector,k_th_bit_S_b);
					_mm256_storeu_si256(S_b_buffer_index,S_b_vector);

	
					__m256i k_th_bit_S_c = _mm256_srli_epi32(next_word,n_shift_c_index);
					k_th_bit_S_c = _mm256_and_si256(k_th_bit_S_c,_mm256_set_epi32(1,1,1,1,1,1,1,1));
					k_th_bit_S_c = _mm256_slli_epi32(k_th_bit_S_c,word_shift_index);
					S_c_vector = _mm256_add_epi32(S_c_vector,k_th_bit_S_c);
					S_c_vector = _mm256_loadu_si256(S_c_buffer_index);
					S_c_vector = _mm256_add_epi32(S_c_vector,k_th_bit_S_c);
					_mm256_storeu_si256(S_c_buffer_index,S_c_vector);


					n_shift_b_index += S_cols;
					n_shift_c_index += S_cols;
					S_b_buffer_index += cl_size;
					S_c_buffer_index += cl_size;

				}
				S_next_word_index += 8;
			}
			word_shift_index -= 1;
		}

		// Loaded and rebuilt 1 cl_block of S
		// Now we can compute the values for fast modulo, the cost of this is amortized since we only do this 
		// ONCE for each row in S
		//TODO can we vectorize this part? how much can we gain from this?
		for(size_t k = 0; k < S_smpls_per_cl_block;++k){ 
			uint32_t L,L2;
			uint32_t d = S_b_buffer[k];
			switch(d){
				case 0: 
					
					continue; // here we just keep going and do not compute the values
					break;
				case 1:
					mod_ops[k].m = 1; mod_ops[k].sh1 = mod_ops[k].sh2 = 0;
					break;
				case 2:  
					mod_ops[k].m = 1; mod_ops[k].sh1 = 1; mod_ops[k].sh2 = 0;
					break;
				default:
					__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );  		//ceil(log2)
					L += 1;												//ceil(log2)
					L2 = (uint32_t) (L < 32? 1 << L : 0);
					mod_ops[k].m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
					mod_ops[k].sh1 = 1; mod_ops[k].sh2 = L-1;
				}

		}






		uint32_t * R_next_word_index = dR;
		for(size_t i = 0; i < R_num_cl_blocks; ++i){
		__m256i R_a_vector;
		__m256i next_word_R;
		memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
			uint32_t word_shift_index = wordsize  - 1;
			for(size_t k  = 0; k < wordsize;++k){
				for(size_t m = 0; m < cl_size; m+= 8){ 

					uint32_t* R_a_buffer_index = R_a_buffer + m;

					next_word_R = _mm256_loadu_si256(R_next_word_index);
					uint32_t n_shift_index = 0;
					for (size_t n = 0; n < R_samples_per_entry;++n){
						__m256i k_th_bit_R_a = _mm256_srli_epi32(next_word_R,n_shift_index);
						k_th_bit_R_a = _mm256_and_si256(k_th_bit_R_a,_mm256_set_epi32(1,1,1,1,1,1,1,1)); 
						k_th_bit_R_a = _mm256_slli_epi32(k_th_bit_R_a,word_shift_index);
						R_a_vector = _mm256_loadu_si256(R_a_buffer_index);
						R_a_vector = _mm256_add_epi32(R_a_vector,k_th_bit_R_a);
						_mm256_store_si256(R_a_buffer_index,R_a_vector);
						

						n_shift_index += R_cols;
						R_a_buffer_index += cl_size;
					}
					R_next_word_index += 8;
				}
				word_shift_index -= 1;
			}


			for(size_t l = 0; l < S_smpls_per_cl_block;++l){
				struct modular_operation mod_l = mod_ops[l];
				uint32_t s_b = S_b_buffer[l];
				uint32_t s_c = S_c_buffer[l];


				__m256i m_v = _mm256_set1_epi32(mod_l.m);
				__m256i d_v = _mm256_set1_epi32(s_b);
				__m256i s_c_v = _mm256_set1_epi32(s_c);


				for(size_t k = 0; k < R_smpls_per_cl_block; k+= 8){
					__m256i x_v = _mm256_loadu_si256(R_a_buffer + k);
					__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
					t_lo = _mm256_srli_epi64(t_lo,32);
					__m256i t_hi = _mm256_srli_epi64(x_v,32);
					t_hi = _mm256_mul_epu32(t_hi,m_v);
					__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
					__m256i res = _mm256_sub_epi32(x_v,t);
					res = _mm256_srli_epi32(res,mod_l.sh1);
					res = _mm256_add_epi32(res,t);
					res = _mm256_srli_epi32(res,mod_l.sh2);
					res = _mm256_mullo_epi32(res,d_v);
					res = _mm256_sub_epi32(x_v,res);
					__m256i eq = _mm256_cmpeq_epi32(res,s_c_v);
					uint32_t* eq_vector = &eq;
		
					for(size_t h = 0; h < 8; ++h){
						if(eq_vector[h] == 4294967295L){
							size_t r_i = ((h + k) % cl_size) * R_samples_per_entry + ((h + k) / cl_size);
							size_t s_j = (l % cl_size) * S_samples_per_entry + (l / cl_size);
							dest[2 * dest_index] = i * R_smpls_per_cl_block + r_i;
							dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + s_j;
							dest_index++;
						}
					}
				}

			}
		}// end cl_block R

	} //end cl_block S



	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	free(mod_ops);
	*dest_rows = dest_index;
}













void q3_weave_index_vertical_block(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){



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



	// ==================== FOR VERTICAL BLOCKS ========================================
	size_t S_n_vertical_blocks  = 1;
	size_t S_vert_block_size = cl_size / S_n_vertical_blocks;
	size_t S_smpls_per_vert_block = S_smpls_per_cl_block /S_n_vertical_blocks; 
	size_t R_n_vertical_blocks = 1;
	size_t R_vert_block_size = cl_size / R_n_vertical_blocks;
	size_t R_smpls_per_vert_block = R_smpls_per_cl_block / R_n_vertical_blocks;

	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_vert_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_vert_block * sizeof(uint32_t));

	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_vert_block * sizeof(uint32_t));


	// ==================== FOR VERTICAL BLOCKS ========================================

	for(size_t j = 0; j < S_num_cl_blocks;++j){

		for(size_t v_b_j = 0; v_b_j < S_n_vertical_blocks;v_b_j++){
			memset(S_b_buffer,0,S_smpls_per_vert_block * 4);
			memset(S_c_buffer,0,S_smpls_per_vert_block * 4);
			for(size_t k  = 0 ; k < wordsize; ++k){
				uint32_t m_index = v_b_j * S_vert_block_size;
				for (size_t m = 0; m < S_vert_block_size; ++m){
					uint32_t next_word = dS[j * cl_block_size + k * cl_size  + m_index ];
					for (size_t n = 0; n < S_samples_per_entry;++n){
									
						uint32_t k_th_bit_S_b = (next_word >> (n * S_cols + 1)) & 1;
						uint32_t k_th_bit_S_c = (next_word >> (n * S_cols + 2)) & 1;

						S_b_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_b << (wordsize - k - 1));
						S_c_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_c << (wordsize - k - 1));
					}
					m_index++;
				}//end vertical block S
			} //end wordsize k 


		

			for(size_t i = 0; i < R_num_cl_blocks; ++i){
				for(size_t v_b_i = 0; v_b_i < R_n_vertical_blocks;v_b_i++){
					memset(R_a_buffer,0,R_smpls_per_vert_block * 4);
					for(size_t k  = 0 ; k < wordsize; ++k){
						uint32_t m_index = v_b_i * R_vert_block_size;
						for (size_t m = 0; m < R_vert_block_size; ++m){
							uint32_t next_word = dR[i * cl_block_size + k * cl_size  + m_index ];
							// printf("Index %d \n",i * cl_block_size + k * cl_size  + m_index );
							for (size_t n = 0; n < R_samples_per_entry;++n){
								uint32_t k_th_bit_R_a = (next_word >> (n * R_cols)) & 1;
								R_a_buffer[m * R_samples_per_entry + n] += (k_th_bit_R_a << (wordsize - k - 1));
							}
							
							m_index++;
						} //end vertical block R


	




					}//end wordsize k


					for(size_t k = 0; k < R_smpls_per_vert_block; ++k){
						for(size_t l = 0; l < S_smpls_per_vert_block; ++l){
							if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
								dest[2 * dest_index] = i * R_smpls_per_cl_block +v_b_i * R_smpls_per_vert_block + k; 
								dest[2 * dest_index + 1] = j * S_smpls_per_cl_block +v_b_j * S_smpls_per_vert_block+  l;
								dest_index++;
							}		 
						}
					}


				}//end all vertical blocks R

			}//end cl_block R


		} //end all vert blocks S
	} //end cl_block S

	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	*dest_rows = dest_index;


}






void q3_weave_index_vertical_block_v2(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
	size_t S_n_vertical_blocks  = 1;
	size_t R_n_vertical_blocks = 1;
	double cycles = 0;
	size_t msr_count = 0;


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



	// ==================== FOR VERTICAL BLOCKS ========================================
	size_t S_vert_block_size = cl_size / S_n_vertical_blocks;
	size_t S_smpls_per_vert_block = S_smpls_per_cl_block /S_n_vertical_blocks; 
	size_t R_vert_block_size = cl_size / R_n_vertical_blocks;
	size_t R_smpls_per_vert_block = R_smpls_per_cl_block / R_n_vertical_blocks;

	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_vert_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_vert_block * sizeof(uint32_t));

	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_vert_block * sizeof(uint32_t));


	// ==================== FOR VERTICAL BLOCKS ========================================
	// size_t S_vert_block_index = 0;
	// size_t S_next_cl_index = 0;
	size_t S_next_block_index = 0;

	
	for(size_t j = 0; j < S_num_cl_blocks;++j){
		size_t S_next_v_block_index = 0;
		int64_t start,end;

		start = start_tsc();

		for(size_t v_b_j = 0; v_b_j < S_n_vertical_blocks;v_b_j++){
			
			size_t S_next_cl_index = S_next_block_index; 
			memset(S_b_buffer,0,S_smpls_per_vert_block * 4);
			memset(S_c_buffer,0,S_smpls_per_vert_block * 4);
			size_t word_shift_index = wordsize - 1 ;
			for(size_t k  = 0 ; k < wordsize; ++k){
				uint32_t S_buffer_index = 0;
				uint32_t m_index = S_next_v_block_index;
				for (size_t m = 0; m < S_vert_block_size; ++m){
					uint32_t next_word = dS[S_next_cl_index + m_index];

					size_t n_shift_b_index = 1;
					size_t n_shift_c_index = 2;
					for (size_t n = 0; n < S_samples_per_entry;++n){
						uint32_t k_th_bit_S_b = (next_word >> n_shift_b_index) & 1;
						uint32_t k_th_bit_S_c = (next_word >> n_shift_c_index) & 1;
						S_b_buffer[S_buffer_index] += (k_th_bit_S_b << word_shift_index);
						S_c_buffer[S_buffer_index] += (k_th_bit_S_c << word_shift_index);
						n_shift_b_index += S_cols;
						n_shift_c_index += S_cols;
						S_buffer_index++;

					}
					m_index++;
					// S_vert_block_index += S_vert_block_size;
				}//end vertical block S
				S_next_cl_index += cl_size;
				word_shift_index--;
			} //end wordsize k 

			end = stop_tsc(start);
			cycles += (double) end;
			msr_count++;

			S_next_v_block_index += S_vert_block_size;
			// PRINT_MALLOC_H(S_b_buffer,R_smpls_per_vert_block);LINE;
		
			size_t R_next_block_index = 0;
			for(size_t i = 0; i < R_num_cl_blocks; ++i){
				size_t R_next_v_block_index = 0;
				for(size_t v_b_i = 0; v_b_i < R_n_vertical_blocks;v_b_i++){
					size_t R_next_cl_index = R_next_block_index; 
					memset(R_a_buffer,0,R_smpls_per_vert_block * 4);
					size_t word_shift_index = wordsize - 1 ;
					for(size_t k  = 0 ; k < wordsize; ++k){
						uint32_t R_buffer_index = 0;
						uint32_t m_index = R_next_v_block_index;
						for (size_t m = 0; m < R_vert_block_size; ++m){
							uint32_t next_word = dR[R_next_cl_index  + m_index ];
							size_t n_shift_a_index = 0;
							for (size_t n = 0; n < R_samples_per_entry;++n){
								uint32_t k_th_bit_R_a = (next_word >> n_shift_a_index) & 1;
								R_a_buffer[R_buffer_index] += (k_th_bit_R_a << word_shift_index);
								R_buffer_index++;
								n_shift_a_index += R_cols;
							}
							
							m_index++;
						} //end vertical block R
						R_next_cl_index += cl_size;
						word_shift_index--;
					}//end wordsize k
					R_next_v_block_index += R_vert_block_size;

					// int64_t start,end;

					// start = start_tsc();

					for(size_t k = 0; k < R_smpls_per_vert_block; ++k){
						for(size_t l = 0; l < S_smpls_per_vert_block; ++l){
							if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
								dest[2 * dest_index] = i * R_smpls_per_cl_block +v_b_i * R_smpls_per_vert_block + k; 
								dest[2 * dest_index + 1] = j * S_smpls_per_cl_block +v_b_j * S_smpls_per_vert_block+  l;
								dest_index++;
							}		 
						}
					}
					// end = stop_tsc(start);
					// cycles += (double) end;
					// msr_count++;



				}//end all vertical blocks R
				R_next_block_index += cl_block_size;
			}//end cl_block R


		} //end all vert blocks S
		S_next_block_index += cl_block_size;
	} //end cl_block S

	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	*dest_rows = dest_index;
	
	printf("Cycles : %lf\n",cycles/msr_count);

}





void q3_weave_index_vertical_block_v3(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
	size_t S_n_vertical_blocks  = 1;
	size_t R_n_vertical_blocks = 1;

	double cycles = 0;
	size_t msr_count = 0;

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



	// ==================== FOR VERTICAL BLOCKS ========================================
	size_t S_vert_block_size = cl_size / S_n_vertical_blocks;
	size_t S_smpls_per_vert_block = S_smpls_per_cl_block /S_n_vertical_blocks; 
	size_t R_vert_block_size = cl_size / R_n_vertical_blocks;
	size_t R_smpls_per_vert_block = R_smpls_per_cl_block / R_n_vertical_blocks;

	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_vert_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_vert_block * sizeof(uint32_t));

	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_vert_block * sizeof(uint32_t));


	// ==================== FOR VERTICAL BLOCKS ========================================
	// size_t S_vert_block_index = 0;
	// size_t S_next_cl_index = 0;

	__m256i next_word;
	__m256i S_b_vector;
	__m256i S_c_vector;
	size_t S_next_block_index = 0;
	int64_t start,end;

	start = start_tsc();
	for(size_t j = 0; j < S_num_cl_blocks;++j){
		size_t S_next_v_block_index = 0;
		for(size_t v_b_j = 0; v_b_j < S_n_vertical_blocks;v_b_j++){
			size_t S_next_cl_index = S_next_block_index; 
			memset(S_b_buffer,0,S_smpls_per_vert_block * 4);
			memset(S_c_buffer,0,S_smpls_per_vert_block * 4);

			size_t word_shift_index = wordsize - 1 ;
			for(size_t k  = 0 ; k < wordsize; ++k){
				uint32_t m_index = S_next_v_block_index;
				for (size_t m = 0; m < S_vert_block_size; m += 8){
					uint32_t S_buffer_index = m;
					// uint32_t next_word = dS[S_next_cl_index + m_index];
					// printf
					next_word = _mm256_loadu_si256(dS + S_next_cl_index + m_index);
					// PRINT_32_BIT_VECTOR(next_word);printf("%u",dS[S_next_cl_index + m_index]);  LINE;
					// printf("%u\n",S_next_cl_index + m_index);
					size_t n_shift_b_index = 1;
					size_t n_shift_c_index = 2;
					for (size_t n = 0; n < S_samples_per_entry;++n){	
						__m256i k_th_bit_S_b = _mm256_srli_epi32(next_word,n_shift_b_index);
						k_th_bit_S_b = _mm256_and_si256(k_th_bit_S_b,_mm256_set_epi32(1,1,1,1,1,1,1,1));
						k_th_bit_S_b = _mm256_slli_epi32(k_th_bit_S_b,word_shift_index);
						S_b_vector = _mm256_loadu_si256(S_b_buffer + S_buffer_index);
						S_b_vector = _mm256_add_epi32(S_b_vector,k_th_bit_S_b);
						_mm256_storeu_si256(S_b_buffer + S_buffer_index,S_b_vector);



						__m256i k_th_bit_S_c = _mm256_srli_epi32(next_word,n_shift_c_index);
						k_th_bit_S_c = _mm256_and_si256(k_th_bit_S_c,_mm256_set_epi32(1,1,1,1,1,1,1,1));
						k_th_bit_S_c = _mm256_slli_epi32(k_th_bit_S_c,word_shift_index);
						S_c_vector = _mm256_loadu_si256(S_c_buffer + S_buffer_index);
						S_c_vector = _mm256_add_epi32(S_c_vector,k_th_bit_S_c);
						_mm256_storeu_si256(S_c_buffer + S_buffer_index,S_c_vector);

						n_shift_b_index += S_cols;
						n_shift_c_index += S_cols;

						S_buffer_index += S_vert_block_size;
					}
					m_index += 8;
					// S_vert_block_index += S_vert_block_size;
				}//end vertical block S
				S_next_cl_index += cl_size;
				word_shift_index--;
			} //end wordsize k 
			S_next_v_block_index += S_vert_block_size;
			end = stop_tsc( start);
			cycles += (double) end;

			msr_count++;

			// PRINT_MALLOC(S_b_buffer,R_smpls_per_vert_block,1);LINE;HLINE;
		
			size_t R_next_block_index = 0;
			__m256i R_a_vector;
			__m256i next_word_R;
			for(size_t i = 0; i < R_num_cl_blocks; ++i){
				size_t R_next_v_block_index = 0;
				for(size_t v_b_i = 0; v_b_i < R_n_vertical_blocks;v_b_i++){
					size_t R_next_cl_index = R_next_block_index; 
					memset(R_a_buffer,0,R_smpls_per_vert_block * 4);
					size_t word_shift_index = wordsize - 1 ;
					for(size_t k  = 0 ; k < wordsize; ++k){
						uint32_t m_index = R_next_v_block_index;
						for (size_t m = 0; m < R_vert_block_size; m += 8){
						uint32_t R_buffer_index = m;
							// uint32_t next_word = dR[R_next_cl_index  + m_index ];
							next_word_R = _mm256_loadu_si256(dR + R_next_cl_index + m_index);
							size_t n_shift_a_index = 0;
							for (size_t n = 0; n < R_samples_per_entry;++n){
								__m256i k_th_bit_R_a = _mm256_srli_epi32(next_word_R,n_shift_a_index);
								k_th_bit_R_a = _mm256_and_si256(k_th_bit_R_a,_mm256_set_epi32(1,1,1,1,1,1,1,1)); 
								k_th_bit_R_a = _mm256_slli_epi32(k_th_bit_R_a,word_shift_index);
								R_a_vector = _mm256_loadu_si256(R_a_buffer + R_buffer_index);
								R_a_vector = _mm256_add_epi32(R_a_vector,k_th_bit_R_a);
								_mm256_store_si256(R_a_buffer + R_buffer_index,R_a_vector);


								R_buffer_index += R_vert_block_size;
								n_shift_a_index += R_cols;
							}
							
							m_index += 8;
						} //end vertical block R
						R_next_cl_index += cl_size;
						word_shift_index--;
					}//end wordsize k
					R_next_v_block_index += R_vert_block_size;
					int64_t start,end;

					start = start_tsc();
					for(size_t k = 0; k < R_smpls_per_vert_block; ++k){
						for(size_t l = 0; l < S_smpls_per_vert_block; ++l){
							if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
								size_t r_i = (k % cl_size) * R_samples_per_entry + (k / cl_size);
								size_t s_j = (l % cl_size) * S_samples_per_entry + (l / cl_size);
								dest[2 * dest_index] = i * R_smpls_per_cl_block + r_i;
								dest[2 * dest_index + 1] = j * S_smpls_per_cl_block +s_j;
								dest_index++;
							}		 
						}
					}

					// end = stop_tsc( start);
					// cycles += (double) end;

					// msr_count++;
				}//end all vertical blocks R
				R_next_block_index += cl_block_size;
			}//end cl_block R


		} //end all vert blocks S
		S_next_block_index += cl_block_size;
	} //end cl_block S

	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	*dest_rows = dest_index;


	printf("Cycles : %lf\n",cycles/msr_count);

}




void q3_fast_recon_fast_modulo(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
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

	struct modular_operation* mod_ops  = (struct modular_operation*) aligned_alloc(32,S_smpls_per_cl_block * sizeof(struct modular_operation));



	__m256i x_07,x_815,x_1623,x_2432,s_c_07,s_c_815,s_c_1623,s_c_2432,s_b_07,s_b_815,s_b_1623,s_b_2432,r_a_07,r_a_815,r_a_1623,r_a_2432,r_a_015,r_a_1632,r_a_all;
	uint32_t* ptr;

	
	__m256i shift_index_07 = _mm256_set_epi32(0,1,2,3,4,5,6,7);
	__m256i shift_index_815 = _mm256_set_epi32(8,9,10,11,12,13,14,15);
	__m256i shift_index_1623 = _mm256_set_epi32(16,17,18,19,20,21,22,23);
	__m256i shift_index_2432 = _mm256_set_epi32(24,25,26,27,28,29,30,31);
	__m256i index = _mm256_setr_epi32(0,16,32,48,64,80,96,112);
	__m256i mask = _mm256_set1_epi32(1);


	for(size_t i = 0; i < S_num_cl_blocks;++i){
		uint32_t * _1bit_index = dS + i * cl_block_size;
		uint32_t * _2bit_index = dS + i * cl_block_size+  cl_size * 8;
		uint32_t * _3bit_index = dS + i * cl_block_size + cl_size * 16;
		uint32_t * _4bit_Index = dS + i * cl_block_size + cl_size * 24;

		// size_t dest_index = 0;
		size_t s_buffer_index = 0;
		for(size_t  j = 0; j < cl_size;++j){

			x_07 = _mm256_i32gather_epi32(_1bit_index,index,4);
			x_815 = _mm256_i32gather_epi32(_2bit_index,index,4);
			x_1623 = _mm256_i32gather_epi32(_3bit_index,index,4);
			x_2432 = _mm256_i32gather_epi32(_4bit_Index,index,4);
			for(size_t k = 0; k < 32;k+=S_cols){
				// printf("%d\n",k);

				// FOR S.b ===================================
				s_b_07 = _mm256_srli_epi32(x_07, k + 1 );
				s_b_815 = _mm256_srli_epi32(x_815, k + 1);
				s_b_1623 = _mm256_srli_epi32(x_1623, k  + 1);
				s_b_2432 = _mm256_srli_epi32(x_2432, k  + 1);

				s_b_07 = _mm256_and_si256(s_b_07,mask);
				s_b_815 = _mm256_and_si256(s_b_815,mask);
				s_b_1623 = _mm256_and_si256(s_b_1623,mask);
				s_b_2432 = _mm256_and_si256(s_b_2432,mask);

				s_b_07 = _mm256_sllv_epi32(s_b_07,shift_index_2432);
				s_b_815 = _mm256_sllv_epi32(s_b_815,shift_index_1623);
				s_b_1623 = _mm256_sllv_epi32(s_b_1623,shift_index_815);
				s_b_2432 = _mm256_sllv_epi32(s_b_2432,shift_index_07);


				__m256i s_b_015 = _mm256_or_si256  (s_b_07,s_b_815);
				__m256i s_b_1632 = _mm256_or_si256  (s_b_1623,s_b_2432);
				__m256i s_b_all = _mm256_or_si256  (s_b_015,s_b_1632);

				ptr = (uint32_t*) &s_b_all;
				uint32_t s_b = 0;
				for(size_t m = 0;m < 8;++m){
					s_b += ptr[m];
				}


				// FOR S.c ===================================
				s_c_07 = _mm256_srli_epi32(x_07, k + 2 );
				s_c_815 = _mm256_srli_epi32(x_815, k + 2);
				s_c_1623 = _mm256_srli_epi32(x_1623, k  + 2);
				s_c_2432 = _mm256_srli_epi32(x_2432, k  + 2);

				s_c_07 = _mm256_and_si256(s_c_07,mask);
				s_c_815 = _mm256_and_si256(s_c_815,mask);
				s_c_1623 = _mm256_and_si256(s_c_1623,mask);
				s_c_2432 = _mm256_and_si256(s_c_2432,mask);

				s_c_07 = _mm256_sllv_epi32(s_c_07,shift_index_2432);
				s_c_815 = _mm256_sllv_epi32(s_c_815,shift_index_1623);
				s_c_1623 = _mm256_sllv_epi32(s_c_1623,shift_index_815);
				s_c_2432 = _mm256_sllv_epi32(s_c_2432,shift_index_07);


				__m256i s_c_015 = _mm256_or_si256  (s_c_07,s_c_815);
				__m256i s_c_1632 = _mm256_or_si256  (s_c_1623,s_c_2432);
				__m256i s_c_all = _mm256_or_si256  (s_c_015,s_c_1632);

				ptr = (uint32_t*) &s_c_all;
				uint32_t s_c = 0;
				for(size_t m = 0;m < 8;++m){
					s_c += ptr[m];
				}

				S_b_buffer[s_buffer_index] = s_b;
				S_c_buffer[s_buffer_index] = s_c;
				s_buffer_index++;
			}
			_1bit_index++;
			_2bit_index++;
			_3bit_index++;
			_4bit_Index++;

		}


		//Computation for fast modulo 

		for(size_t k = 0; k < S_smpls_per_cl_block;++k){ 
			uint32_t L,L2;
			uint32_t d = S_b_buffer[k];
			switch(d){
				case 0: 
					
					continue; // here we just keep going and do not compute the values
					break;
				case 1:
					mod_ops[k].m = 1; mod_ops[k].sh1 = mod_ops[k].sh2 = 0;
					break;
				case 2:  
					mod_ops[k].m = 1; mod_ops[k].sh1 = 1; mod_ops[k].sh2 = 0;
					break;
				default:
					__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );  		//ceil(log2)
					L += 1;												//ceil(log2)
					L2 = (uint32_t) (L < 32? 1 << L : 0);
					mod_ops[k].m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
					mod_ops[k].sh1 = 1; mod_ops[k].sh2 = L-1;
				}

		}

		// Reconcstruct all R values:


		for(size_t p = 0; p < R_num_cl_blocks;++p){
			_1bit_index = dR + p * cl_block_size;
			_2bit_index = dR + p * cl_block_size+  cl_size * 8;
			_3bit_index = dR + p * cl_block_size + cl_size * 16;
			_4bit_Index = dR + p * cl_block_size + cl_size * 24;
			size_t r_a_buffer_index = 0;
			for(size_t o = 0; o < cl_size; ++o){

				x_07 = _mm256_i32gather_epi32(_1bit_index,index,4);
				x_815 = _mm256_i32gather_epi32(_2bit_index,index,4);
				x_1623 = _mm256_i32gather_epi32(_3bit_index,index,4);
				x_2432 = _mm256_i32gather_epi32(_4bit_Index,index,4);
				for(size_t k = 0; k < 32;k+=R_cols){
					r_a_07 = _mm256_srli_epi32(x_07, k );
					r_a_815 = _mm256_srli_epi32(x_815, k);
					r_a_1623 = _mm256_srli_epi32(x_1623, k );
					r_a_2432 = _mm256_srli_epi32(x_2432, k );


					r_a_07 = _mm256_and_si256(r_a_07,mask);
					r_a_815 = _mm256_and_si256(r_a_815,mask);
					r_a_1623 = _mm256_and_si256(r_a_1623,mask);
					r_a_2432 = _mm256_and_si256(r_a_2432,mask);


					r_a_07 = _mm256_sllv_epi32(r_a_07,shift_index_2432);
					r_a_815 = _mm256_sllv_epi32(r_a_815,shift_index_1623);
					r_a_1623 = _mm256_sllv_epi32(r_a_1623,shift_index_815);
					r_a_2432 = _mm256_sllv_epi32(r_a_2432,shift_index_07);

					r_a_015 = _mm256_or_si256  (r_a_07,r_a_815);
					r_a_1632 = _mm256_or_si256  (r_a_1623,r_a_2432);
					r_a_all = _mm256_or_si256  (r_a_015,r_a_1632);

					ptr = (uint32_t*) &r_a_all;
					uint32_t r_a = 0;
					for(size_t m = 0;m < 8;++m){
						r_a += ptr[m];
					}
					R_a_buffer[r_a_buffer_index] = r_a;
					r_a_buffer_index++;
				}

				_1bit_index++;
				_2bit_index++;
				_3bit_index++;
				_4bit_Index++;
			}
			
			// PRINT_MALLOC_H(S_b_buffer,S_smpls_per_cl_block);LINE;
			// PRINT_MALLOC_H(R_a_buffer,R_smpls_per_cl_block);LINE;
			// PRINT_MALLOC_H(S_c_buffer,S_smpls_per_cl_block);LINE;

			for(size_t l = 0; l < S_smpls_per_cl_block;++l){
				struct modular_operation mod_l = mod_ops[l];
				uint32_t s_b = S_b_buffer[l];
				uint32_t s_c = S_c_buffer[l];
				if(s_b == 0 ){continue;}
				__m256i m_v = _mm256_set1_epi32(mod_l.m);
				__m256i d_v = _mm256_set1_epi32(s_b);
				__m256i s_c_v = _mm256_set1_epi32(s_c);
				


				for(size_t k = 0; k < R_smpls_per_cl_block; k+= 8){
					__m256i x_v = _mm256_loadu_si256(R_a_buffer + k);
					__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
					t_lo = _mm256_srli_epi64(t_lo,32);
					__m256i t_hi = _mm256_srli_epi64(x_v,32);
					t_hi = _mm256_mul_epu32(t_hi,m_v);
					__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
					__m256i res = _mm256_sub_epi32(x_v,t);
					res = _mm256_srli_epi32(res,mod_l.sh1);
					res = _mm256_add_epi32(res,t);
					res = _mm256_srli_epi32(res,mod_l.sh2);
					res = _mm256_mullo_epi32(res,d_v);
					res = _mm256_sub_epi32(x_v,res);
					__m256i eq = _mm256_cmpeq_epi32(res,s_c_v);
					uint32_t* eq_vector = &eq;

					for(size_t h = 0; h < 8; ++h){
						if(eq_vector[h] == 4294967295L){
	
							dest[2 * dest_index] = p * R_smpls_per_cl_block + k + h;
							dest[2 * dest_index + 1] = i * S_smpls_per_cl_block + l;
							dest_index++;
						}
					}
				
				
				}
			}

		} //end cl_bock R






		// PRINT_MALLOC(S_c_buffer,S_smpls_per_cl_block,1);
	} //end cl_block S

	*dest_rows = dest_index;
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	free(mod_ops);
}



