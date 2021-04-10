// #include <stdint.h> 
// #include <stdio.h>
// #include <stdlib.h>
// #include "../include/ASL/debug.h"

// #define CL 8
// #define M  8 //Word size
// #define B  2 //Number of Banks 
// #define R  2 // rows







    
// // #define GET_NTH_BIT(n) = 

//   int main(int argc, char **argv) {
	
//     int BITS_PER_BANK = CL / B;

//     // unsigned char res = 0b10000000;
//     // PRINT_B(res);
//     // int rows = 2;
//     unsigned char p[16] = {152,2,3,4,5,7,23,152,9,10,11,12,13,14,15,16};
//     unsigned char r[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

//     COMPACT_NTH_BIT(0,p[0],r[0]);
//     COMPACT_NTH_BIT(0,p[1],r[0]);
//     COMPACT_NTH_BIT(0,p[2],r[0]);
//     COMPACT_NTH_BIT(0,p[3],r[0]);


//     COMPACT_NTH_BIT(0,p[8],r[0]);
//     COMPACT_NTH_BIT(0,p[9],r[0]);
//     COMPACT_NTH_BIT(0,p[10],r[0]);
//     COMPACT_NTH_BIT(0,p[11],r[0]);

//     // for (size_t k = 0; k < R; k++)
//     // {
//     //     for (size_t i = 0; i < B; i++)
//     //     {
//     //         for (size_t j = 0; j < BITS_PER_BANK; j++)
//     //         {
//     //             COMPACT_NTH_BIT(,res[])
//     //         }
            
//     //     }
        
//     // }
    


//     // for (size_t k = 0; k < 2; k++) //amount of rows
//     // {   
//     //     for (size_t i = 0; i < M; i++)
//     //     {
//     //         for (size_t j = 0; j < M; j++)
//     //         {
//     //         COMPACT_NTH_BIT(i,p[k * M + j],r[k*M + i]);
//     //         }
//     //     }
//     // }

//     PRINT_ARR(16,p,1);
//     LINE;
//     PRINT_ARR(16,r,1);
    

//     // for(int i = 0; i < B;i++){


//     // // }
//     // unsigned char res = 0b00000000;
//     // unsigned char test = 0b11001001;
//     // COMPACT_NTH_BIT(7,test,res);
//     // DEBUG_B(res);
//     // unsigned char test3 = 0b00100010;
//     // unsigned char tes1 = res >> 1; 
//     // unsigned char res2 = test3 << 2 & 0b10000000;

//     // printf(BYTE_TO_BINARY_PATTERN"\n",BYTE_TO_BINARY(res));
//     // printf(BYTE_TO_BINARY_PATTERN"\n",BYTE_TO_BINARY(tes1));
//     // printf(BYTE_TO_BINARY_PATTERN"\n",BYTE_TO_BINARY(res2));


//     // unsigned char res1 = 0b11000000;
//     // unsigned char res2 = 0b01000000;
//     // unsigned char res3 = 0b00000000;
//     // unsigned char res4 = 0b11000000;
//     // unsigned char res5 = 0b01000000;
//     // unsigned char res6 = 0b10000000;
//     // unsigned char res7 = 0b00000000;
//     // unsigned char res8 = 0b11000000;
    
//     // unsigned char res = 0b00000000;

//     // COMPACT_NTH_BIT(1,res,res1);
//     // COMPACT_NTH_BIT(1,res,res2);
//     // COMPACT_NTH_BIT(1,res,res3);
//     // COMPACT_NTH_BIT(1,res,res4);
//     // COMPACT_NTH_BIT(1,res,res5);
//     // COMPACT_NTH_BIT(1,res,res6);
//     // COMPACT_NTH_BIT(1,res,res7);
//     // COMPACT_NTH_BIT(1,res,res8);
    
//     // printf(BYTE_TO_BINARY_PATTERN"\n",BYTE_TO_BINARY(res));
// 	return 0;
// }