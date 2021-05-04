#ifndef DEBUG_H_
#define DEBUG_H_

#include <stdio.h>


#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define BLU   "\x1B[34m"
#define YEL   "\x1B[33m"
#define RESET "\x1B[0m"
// credit for defines here : https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#define PRINT_B(byte) \
    printf(BYTE_TO_BINARY_PATTERN " ",BYTE_TO_BINARY(byte)); 

#define PRINT_32_B(int32)\
        PRINT_B(int32 >> 24);\
        PRINT_B(int32 >> 16);\
        PRINT_B(int32 >> 8);\
        PRINT_B(int32);\
		
#define PRINT_64_B(int64)\
		PRINT_B(int64 >> 56);\
        PRINT_B(int64 >> 48);\
        PRINT_B(int64 >> 40);\
        PRINT_B(int64 >> 32);\
        PRINT_B(int64 >> 24);\
        PRINT_B(int64 >> 16);\
        PRINT_B(int64 >> 8);\
        PRINT_B(int64);\

#define LINE \
    printf("\n");

#define BORDER\
    printf(" | ");

#define HLINE \
    printf("==============================================================================================================================================================\n");
#define HBORDER \
    printf("--------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

#define DEBUG_B(byte) \
    printf(BYTE_TO_BINARY_PATTERN " ",BYTE_TO_BINARY(byte));\
    LINE;

void PRINT_MALLOC(uint32_t* ptr, size_t rows, size_t cols);

void PRINT_MALLOC_B(uint32_t *ptr,size_t rows,size_t cols);

void PRINT_WEAVED(uint32_t* ptr,size_t rows, size_t cols);
void PRINT_MALLOC_H(uint32_t* ptr,size_t n);
#endif