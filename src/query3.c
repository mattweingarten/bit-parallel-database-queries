
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query3.h"





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