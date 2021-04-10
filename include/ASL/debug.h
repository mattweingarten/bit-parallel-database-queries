#include <stdio.h>


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

#define LINE \
    printf("\n");

#define DEBUG_B(byte) \
    printf(BYTE_TO_BINARY_PATTERN " ",BYTE_TO_BINARY(byte));\
    LINE;

#define COMPACT_NTH_BIT(n,input,res)  \
    res = res << 1;                   \
    res = res | input >> M - n -1  & 1 ; 


#define PRINT_ARR(n,p,l) \
    for (size_t i = 0; i < n; i++) \
    {\
       if(i % l == 0){LINE};\
       PRINT_B(p[i]);\
    }\
    LINE

#define PRINT_DB(n,p) 