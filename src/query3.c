
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
void q3(uint32_t *dR, uint32_t *dS, uint32_t * dest, int R_rows, int R_cols, int S_rows, int S_cols){
    for(int i = 0; i < R_rows; i++){
		for(int j = 0; j < S_rows; j++){
				if(dR[i * R_cols] % dS[j * S_cols + 1] == dS[j * S_cols + 2]){
					dest[i * S_rows + j] = 1;
				} else {
					dest[i * S_rows + j] = 0;
				}
		}
	}
}