
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query2.h"

//Select SUM(c) FROM R WHERE R.a < R.b

//Straightforward
//TODO

uint64_t q2(uint32_t * data,int rows,int cols){
    uint64_t sum = 0;
    for(int i = 0;i < rows;++i){
        if(data[i * cols + 0] < data[i * cols + 1]){
            sum += data[i * cols + 2];
        }
    }
    return sum;
}