
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query3.h"



uint32_t cartesian_product_size(int R_rows, int R_cols, int S_rows, int S_cols){
	return R_rows * S_rows * (R_cols + S_cols);
}

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


// void q3_weave(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
// 	size_t R_words_per_cl = 32 * cl_size / R_cols;  
// 	size_t S_words_per_Cl = 32 * cl_size / S_cols;
// 	for(size_t i = 0; i < R_rows; ++i){
// 		uint32_t R_a = 0;
// 		for(size_t k = 0;k < wordsize; ++k){

// 		}
// 		for(size_t j = 0; j < S_rows;++j){

// 		}
// 	}

// }