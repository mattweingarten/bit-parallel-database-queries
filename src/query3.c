
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
    for(int i = 0; i < R_rows; i++){
		for(int j = 0; j < S_rows; j++){
			if(dS[j * S_cols + 1] != 0 && dR[i * R_cols + 0] % dS[j * S_cols + 1] == dS[j * S_cols + 2]){
				for(k = 0; k < R_cols; ++k){
					dest[dest_row *  dest_cols + k] = dR[i * R_cols + k];
				}
				for(int l = 0; l < S_cols; ++l){
					dest[dest_row *  dest_cols + k + l] = dS[i * S_cols + l];
				}
				dest_row++;
			}
		}
	}
	*dest_rows = dest_row;
}