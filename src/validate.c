
#include "../include/validate.h"
#include "../include/debug.h"
#include "../include/generator.h"

#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include<stdbool.h>  
#include <stdarg.h>

void single_test(union query_t query, enum Query type){

    generator gen = &rand_gen;
    size_t rows = 512;
    size_t cols = 4;
    bool correct;
    size_t count_correct = 0;
    size_t count = 0;

	switch (type)
	{
	case Q1:
		correct = true;
        uint32_t * db = generateDB(rows,cols,gen);
        uint32_t * gt =  q1_groundtruth(db,rows,cols);
        uint32_t * ml = weave_samples_wrapper(db,rows,cols);
        uint32_t * res = q1_wrapper(query.q1,ml,rows,cols);
        correct = correct && compare(res,gt,rows);
        free(db);
        free(gt);
        free(ml);
        free(res);
		break;
	case Q2:
		correct = test_q2((q2_t) query.q2,gen,rows,cols);
		break;
	case Q3:
		break;
	default:
		printf("Invalid query type!\n");
		break;
	}

	if(correct){
		count_correct++;
		printf(GRN "PASSED" RESET);
		printf(" Test for q%d  with rows =  %d, cols = %d\n",type + 1,rows,cols);
	}else{
		printf(RED "FAILED" RESET);
		printf(" Test for q%d  with rows =  %d, cols = %d\n",type  + 1,rows,cols);
	}
	count++;
       
     
    
    printf("\n\n======================== Validation completed PASSED: %d/%d ==========================\n",count_correct,count);
}



void validate_query(union query_t query, enum Query type){

    generator generators[5] = {&rand_gen,&asc_gen,&i_gen,&j_gen,&mod_gen};
    size_t row_sizes[5] = {128,256,512,2048,32768};
    size_t cols_sizes[4] = {4,8,16,32};
    bool correct;
    size_t count_correct = 0;
    size_t count = 0;
    printf("======================== Starting validation ==========================\n\n");
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0;k < 4;k++){
                bool correct;

                switch (type)
                {
                case Q1:
                    correct = test_q1(query.q1,generators[i],row_sizes[j],cols_sizes[k]);
                    break;
                case Q2:
                    correct = test_q2(query.q2,generators[i],row_sizes[j],cols_sizes[k]);
                    break;
                case Q3:
                    break;
                default:
                    printf("Invalid query type!\n");
                    break;
                }

                if(correct){
                    count_correct++;
                    printf(GRN "PASSED" RESET);
                    printf(" Test for q%d  with rows =  %d, cols = %d, gen = %d\n",type + 1,row_sizes[j],cols_sizes[k],i);
                }else{
                    printf(RED "FAILED" RESET);
                    printf(" Test for q%d  with rows =  %d, cols = %d, gen = %d\n",type  + 1,row_sizes[j],cols_sizes[k],i);
                }
                count++;
            }
        }
    }
    printf("\n\n======================== Validation completed PASSED: %d/%d ==========================\n",count_correct,count);
}

bool test_q1(q1_t q,generator gen,size_t rows,size_t cols){
    bool correct = true;
    for(size_t i = 0; i < N_RUNS; ++i){
        uint32_t * db = generateDB(rows,cols,gen);
        uint32_t * gt =  q1_groundtruth(db,rows,cols);
        uint32_t * ml = weave_samples_wrapper(db,rows,cols);
        uint32_t * res = q1_wrapper(q,ml,rows,cols);
        correct = correct && compare(res,gt,rows);
        free(db);
        free(gt);
        free(ml);
        free(res);
    }
    return correct;
}


bool test_q2(q2_t q,generator gen,size_t rows,size_t cols){
    bool correct = true;
    for(size_t i = 0; i < N_RUNS; ++i){
        uint32_t * db = generateDB(rows,cols,gen);
        uint64_t  gt =  q2_groundtruth(db,rows,cols);
        uint32_t * ml = weave_samples_wrapper(db,rows,cols);
        uint64_t res = q2_wrapper(q,ml,rows,cols);
        correct = correct && res == gt;
        free(db);
        free(ml);
    }
    return correct;
}



bool compare(uint32_t * x, uint32_t *y,size_t n){
    bool res = true;
    for(int i = 0; i < n; ++i){
        res = res && (x[i] == y[i]);
    }
    return res;
}


//groundtruths

uint32_t* q1_groundtruth(uint32_t* data,size_t rows,size_t cols){
    uint32_t* res = malloc(rows * sizeof(uint32_t));
    q1(data,res,rows,cols);
    return res;
}

uint64_t q2_groundtruth(uint32_t* data,size_t rows,size_t cols){
    return q2(data,rows,cols);
}


//wrapper functions
uint32_t *weave_samples_wrapper(uint32_t* data,size_t rows,size_t cols){
    size_t numEntries = numberOfEntries(rows,cols);
    uint32_t * res = (uint32_t* ) malloc(numEntries * sizeof(uint32_t));
    weave_samples(res,data,rows,cols);
    return res;
}

uint32_t *q1_wrapper(q1_t q,uint32_t* data,size_t rows,size_t cols){
    uint32_t* results = malloc(rows * sizeof(uint32_t));
    uint32_t * temps = malloc(rows * sizeof(uint32_t));
    memset(results,0,rows*4);
	memset(temps,0,rows*4);
    size_t numEntries = numberOfEntries(rows,cols);
    q1_parallel_weave(data,results,temps,32,512,cols,rows,numEntries);
    free(temps);
    return results;
}

uint64_t q2_wrapper(q2_t q,uint32_t* data,size_t rows,size_t cols){
    uint32_t samples_per_block = 512 / cols;
    uint32_t * cond_buffer = malloc(samples_per_block * sizeof(uint32_t));
    uint32_t * temp_buffer = malloc(samples_per_block * sizeof(uint32_t));
    uint32_t * sum_buffer = malloc(samples_per_block * sizeof(uint32_t));
    memset(cond_buffer,0,samples_per_block*4);
	memset(temp_buffer,0,samples_per_block*4);
	memset(sum_buffer,0,samples_per_block*4);
    size_t numEntries = numberOfEntries(rows,cols);
    uint64_t res = q(data,cond_buffer,temp_buffer,sum_buffer,32,512,cols,rows,numEntries);
    free(cond_buffer);
    free(temp_buffer);
    free(sum_buffer);
    return res;
}