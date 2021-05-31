

#include "../include/converter.h"
#include "../include/validate.h"
#include "../include/debug.h"
#include "../include/generator.h"

#include <stdio.h>
#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>  
#include <stdarg.h>
#include <math.h>
#include <x86intrin.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

void validate_query(void* query, enum Query type){

    generator generators[5] = {&rand_gen,&asc_gen,&i_gen,&j_gen,&mod_gen};
    size_t row_sizes[7] = {128,256,512,2048,4096,65536,131072};
	size_t cols_sizes[4] = {2,8,16,32};
	if(type == Q2)
		cols_sizes[0] = 4;
     
    // TODO: DB with 64 columns break
    bool correct;
    size_t count_correct = 0;
    size_t count = 0;
    printf("======================== Starting validation ==========================\n\n");
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 7; j++){
            for(int k = 0;k < 4;k++){
				if(cols_sizes[k] * row_sizes[j] < 512) continue;
                bool correct;

                switch (type)
                {
                case Q1:
                    correct = test_q1((q1_t) query ,generators[i],row_sizes[j],cols_sizes[k]);
                    if(correct){
                        count_correct++;
                        printf(GRN "PASSED" RESET);
                        printf(" Test for q%d  with rows =  %d, cols = %d, gen = %d\n",type + 1,row_sizes[j],cols_sizes[k],i);
                    }else{
                        printf(RED "FAILED" RESET);
                        printf(" Test for q%d  with rows =  %d, cols = %d, gen = %d\n",type  + 1,row_sizes[j],cols_sizes[k],i);
                    }
                    count++;
                    break;
                case Q2:
                    correct = test_q2((q2_t) query,generators[i],row_sizes[j],cols_sizes[k]);
                        if(correct){
                            count_correct++;
                            printf(GRN "PASSED" RESET);
                            printf(" Test for q%d  with rows =  %d, cols = %d, gen = %d\n",type + 1,row_sizes[j],cols_sizes[k],i);
                        }else{
                            printf(RED "FAILED" RESET);
                            printf(" Test for q%d  with rows =  %d, cols = %d, gen = %d\n",type  + 1,row_sizes[j],cols_sizes[k],i);
                        }
                    count++;
                    break;
                case Q3:
                    correct = test_q3((q3_t)query,&rand_gen,&rand_100_gen,row_sizes[j  % 2 + 3],cols_sizes[k],row_sizes[j % 3 + 1],cols_sizes[k % 2 + 1]);
                    if(correct){
                        count_correct++;
                        printf(GRN "PASSED" RESET);
                    }else{
                        printf(RED "FAILED" RESET);
                    }
                    count++;
                    printf(" Test for q%d  with R_rows = %d, R_cols = %d, R_gen = randgen, S_rows = %d, S_cols = %d,S_gen=mod_gen\n",type  + 1,row_sizes[j],cols_sizes[k],row_sizes[j % 3 + 1],cols_sizes[k % 2 + 1]);
                    break;
                


                case Q3b:
                    //TODO: make this smarter here
                    // for(size_t b_size = 1; b_size < )
                    correct = test_q3_block((q3b_t) query,&rand_gen,&mod_gen,row_sizes[j  % 2 + 3],cols_sizes[k],row_sizes[j % 3 + 1],cols_sizes[k % 2 + 1],4);
                    if(correct){
                        count_correct++;
                        printf(GRN "PASSED" RESET);
                    }else{
                        printf(RED "FAILED" RESET);
                    }

                    count++;
                    printf(" Test for q%d  with R_rows = %d, R_cols = %d, R_gen = randgen, S_rows = %d, S_cols = %d,S_gen=mod_gen,block_size  = %d\n",type  + 1,row_sizes[j  % 2 + 3],cols_sizes[k],row_sizes[j % 3 + 1],cols_sizes[k % 2 + 1],4);
                    break;
                default:
                    printf("Invalid query type!\n");
                    break;
                }


            }
        }
    }
    printf("\n\n======================== Validation completed PASSED:" YEL " %d/%d " RESET  "==========================\n",count_correct,count);
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


bool test_q3(q3_t q, generator R_gen,generator S_gen,size_t R_rows,size_t R_cols, size_t S_rows,size_t S_cols){
    bool correct = true;
    for(size_t i = 0; i < N_RUNS;++i){
        uint32_t* R = generateDB(R_rows,R_cols,R_gen);
        uint32_t* S = generateDB(S_rows,S_cols,S_gen);

        // PRINT_MALLOC( 32, S,S_rows,S_cols);
        // HLINE;

        // PRINT_MALLOC(R,R_rows,R_cols);
        // PRINT_MALLOC(S,S_rows,S_cols);

        // // // PRINT_aligned_alloc( 32, R,R_rows,R_cols);
        // HLINE;

        uint32_t * R_weave = weave_samples_wrapper(R,R_rows,R_cols);
        uint32_t * S_weave = weave_samples_wrapper(S,S_rows,S_cols);
        // PRINT_WEAVED(R_weave,256,4);
        // HLINE;
        // PRINT_aligned_alloc( 32, R,R_rows,R_cols);
        uint32_t gt_out_size = cart_prod(R_rows,S_rows);
        uint32_t* gt = (uint32_t*) aligned_alloc( 32, gt_out_size * 2 * sizeof(uint32_t));

        q3_index(R,S,gt,&gt_out_size,R_rows,R_cols,S_rows,S_cols);

        uint32_t* re_gt = realloc(gt,gt_out_size * 2 *  sizeof(uint32_t));
        
        free(R);
        free(S);

        


        

        uint32_t comp_out_size = cart_prod(R_rows,S_rows);
        uint32_t* comp = (uint32_t*) aligned_alloc( 32, comp_out_size * 2 * sizeof(uint32_t));

        q(R_weave,S_weave,comp,&comp_out_size,R_rows,R_cols,S_rows,S_cols,32,16);

        uint32_t* re_comp = realloc(comp,comp_out_size * 2 *  sizeof(uint32_t));

        correct = compare_join(re_gt,re_comp,gt_out_size,comp_out_size);
        // printf("Outsize: %d  -> ",gt_out_size);
    

        free(R_weave);
        free(S_weave);
        free(re_comp);
        free(re_gt);
    }
    return correct;
}




bool test_q3_block(q3b_t q, generator R_gen,generator S_gen,size_t R_rows,size_t R_cols, size_t S_rows,size_t S_cols,size_t block_size){
    bool correct = true;
    for(size_t i = 0; i < N_RUNS;++i){
        uint32_t* R = generateDB(R_rows,R_cols,R_gen);
        uint32_t* S = generateDB(S_rows,S_cols,S_gen);

        // PRINT_aligned_alloc( 32, S,S_rows,S_cols);
        // HLINE;

        // PRINT_MALLOC(R,R_rows,R_cols);
        // PRINT_MALLOC(S,S_rows,S_cols);
        // PRINT_aligned_alloc( 32, R,R_rows,R_cols);
        // HLINE;

        uint32_t * R_weave = weave_samples_wrapper(R,R_rows,R_cols);
        uint32_t * S_weave = weave_samples_wrapper(S,S_rows,S_cols);
        // PRINT_WEAVED(R_weave,256,4);
        // HLINE;
        // PRINT_aligned_alloc( 32, R,R_rows,R_cols);
        uint32_t gt_out_size = cart_prod(R_rows,S_rows);
        uint32_t* gt = (uint32_t*) aligned_alloc( 32, gt_out_size * 2 * sizeof(uint32_t));

        q3_index(R,S,gt,&gt_out_size,R_rows,R_cols,S_rows,S_cols);

        uint32_t* re_gt = realloc(gt,gt_out_size * 2 *  sizeof(uint32_t));
        
        free(R);
        free(S);

        


        

        uint32_t comp_out_size = cart_prod(R_rows,S_rows);
        uint32_t* comp = (uint32_t*) aligned_alloc( 32, comp_out_size * 2 * sizeof(uint32_t));

        q(R_weave,S_weave,comp,&comp_out_size,R_rows,R_cols,S_rows,S_cols,32,16,block_size);

        uint32_t* re_comp = realloc(comp,comp_out_size * 2 *  sizeof(uint32_t));

        correct = compare_join(re_gt,re_comp,gt_out_size,comp_out_size);
        
    

        free(R_weave);
        free(S_weave);
        free(re_comp);
        free(re_gt);
    }
    return correct;
}



//groundtruths

uint32_t* q1_groundtruth(uint32_t* data,size_t rows,size_t cols){
    uint32_t* res = aligned_alloc( 32, rows * sizeof(uint32_t));
    q1(data,res,rows,cols);
    return res;
}

uint64_t q2_groundtruth(uint32_t* data,size_t rows,size_t cols){
    return q2(data,rows,cols);
}




uint32_t *q1_wrapper(q1_t q,uint32_t* data,size_t rows,size_t cols){
    
    uint32_t* results = aligned_alloc( 32, rows * sizeof(uint32_t));
    uint32_t *temps = aligned_alloc( 32, rows * sizeof(uint32_t));
    

    for(size_t i = 0 ; i < rows; ++i ){
        results[i]  = 0;
        temps[i] = 0;
    }
    // memset(results,0,rows*4);
	// memset(temps,0,rows*4);
    
    size_t numEntries = numberOfEntries(rows,cols);
    int64_t empty_cycles = 0;
    q(data,results,temps,&empty_cycles,&empty_cycles,rows,cols,numEntries);
    free(temps);
    return results;
}

uint64_t q2_wrapper(q2_t q,uint32_t* data,size_t rows,size_t cols){
    uint32_t samples_per_block = 512 / cols;
    uint32_t * cond_buffer = aligned_alloc( 32, samples_per_block * sizeof(uint32_t));
    uint32_t * temp_buffer = aligned_alloc( 32, samples_per_block * sizeof(uint32_t));
    uint32_t * sum_buffer = aligned_alloc( 32, samples_per_block * sizeof(uint32_t));
    memset(cond_buffer,0,samples_per_block*4);
	memset(temp_buffer,0,samples_per_block*4);
	memset(sum_buffer,0,samples_per_block*4);
    size_t numEntries = numberOfEntries(rows,cols);
    uint64_t res = q(data,cond_buffer,temp_buffer,sum_buffer,32,512,rows,cols,numEntries);
    free(cond_buffer);
    free(temp_buffer);
    free(sum_buffer);
    return res;
}


// void q3_wrapper(q3_t q,uint32_t *data,size_t rows,size_t cols){
//     uint32_t 
// }

bool compare(uint32_t * x, uint32_t *y,size_t n){
    bool res = true;
    for(int i = 0; i < n; ++i){
        res = res && (x[i] == y[i]);
    }
    return res;
}


bool compare_rows_cols(uint32_t * x, uint32_t *y,size_t rows,size_t cols){
    bool res = true;
    // printf("Got here\n");
    for(int i = 0; i < rows; ++i){
        // bool row_res = true;
        for(int j = 0; j < cols;++j){
            uint32_t xij = x[i * cols + j];
            uint32_t yij =  y[i * cols + j];
            res = res && xij == yij;
            // row_res = row_res && xij == yij;

        }
        if(false){
            printf("[%d] =>",i);
            for(int j = 0; j < cols; ++j){
                uint32_t xij = x[i * cols + j];
                uint32_t yij =  y[i * cols + j];
                printf(" %u ",xij);
            }
            for(int j = 0; j < cols; ++j){
                uint32_t xij = x[i * cols + j];
                uint32_t yij =  y[i * cols + j];
                if(xij == yij){
                    printf(GRN " %u " RESET ,yij);
                }else{
                    printf(RED " %u " RESET ,yij);
                }
                    
            }
            printf("\n");
            
        }
    }
    return res;
}


bool compare_join(uint32_t * x, uint32_t *y,size_t x_rows,size_t y_rows){
    if(x_rows != y_rows){
        
        printf("Size of joins are" RED " NOT " RESET "equal: %u != %u\n",x_rows,y_rows);
        return false;
    }
    for(size_t i = 0 ; i < x_rows; ++i){
        bool found = false;
        for(size_t j = 0; j < x_rows;++j){
            if(x[i * 2] == y[j * 2] && x[i * 2 + 1] == y[j * 2 + 1]){
                found = true;
            }
        }
        if(!found){
            printf("Output row %u was" RED " NOT " RESET "found in second result: (value = %u,%u)\n",i,x[i * 2], x[i * 2 + 1]);
            return false;
        }
 
    }
    return true;

}