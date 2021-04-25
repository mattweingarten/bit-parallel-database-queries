#ifndef DEBUG
#define DEBUG

#include <stdio.h>

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

#define LINE \
    printf("\n");

#define BORDER\
    printf(" | ");

#define DEBUG_B(byte) \
    printf(BYTE_TO_BINARY_PATTERN " ",BYTE_TO_BINARY(byte));\
    LINE;

void PRINT_MALLOC(uint32_t* ptr, size_t rows, size_t cols);

void PRINT_MALLOC_B(uint32_t *ptr,size_t rows,size_t cols);

void PRINT_WEAVED(uint32_t* ptr,size_t rows, size_t cols);
#endif