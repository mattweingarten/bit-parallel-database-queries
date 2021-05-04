

#include "../include/converter.h"
#include "../include/validate.h"
#include "../include/perform.h"
#include "../include/debug.h"
#include "../include/tsc_x86.h"
#include "../include/generator.h"

#include <stdint.h> 
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>  
#include <stdarg.h>
#include <math.h>


#define MAX(a,b) (((a)>(b))?(a):(b))

void performance_rnd_query(void* query, enum Query type){
    generator generators[5] = {&rand_gen,&asc_gen,&i_gen,&j_gen,&mod_gen};
    size_t row_sizes[5] = {128,256,512,2048,32768};
    size_t cols_sizes[4] = {4,8,16,32};
    //bool correct;
		double cycles;
		int cyc;
    size_t count_correct = 0;
    size_t count = 0;
		
    printf("======================== Starting Performance Test on Random Data ======================\n\n");
    for(int i = 0; i < 5; i++){
        for(int j = 0; j < 5; j++){
            for(int k = 0;k < 4;k++){
                bool correct;

                switch (type)
                {
                case Q1:
										cycles = perf_test_q1((q1_t) query ,generators[i],row_sizes[j],cols_sizes[k]);
										cyc = cycles;
										if(PRINT_CYCLES)
											printf("%d cycles for q%d  with rows =  %d, cols = %d, gen = %d\n",cyc, type + 1,row_sizes[j],cols_sizes[k],i);
										saveCycledataToFile("./q1_cycles.csv",cycles,row_sizes[j],cols_sizes[k],i);
                    break;
                case Q2:
										cycles = perf_test_q2((q2_t) query ,generators[i],row_sizes[j],cols_sizes[k]);
										cyc = cycles;
										if(PRINT_CYCLES)
											printf("%d cycles for q%d  with rows =  %d, cols = %d, gen = %d\n",cyc, type + 1,row_sizes[j],cols_sizes[k],i);
										saveCycledataToFile("./q2_cycles.csv",cycles,row_sizes[j],cols_sizes[k],i);
                    break;
                case Q3:
                    printf(" Test for q%d  with R_rows = %d, R_cols = %d, R_gen = randgen, S_rows = %d, S_cols = %d,S_gen=mod_gen\n",type  + 1,row_sizes[j],cols_sizes[k],row_sizes[j % 2],cols_sizes[k % 2]);
                    break;
                default:
                    printf("Invalid query type!\n");
                    break;
                }


            }
        }
    }
    printf("\n\n======================== Performance Test completed ==========================\n");
}

double perf_test_q1(q1_t q,generator gen,size_t rows,size_t cols){
    bool correct = true;
		uint32_t * db = generateDB(rows,cols,gen);
		uint32_t * gt = q1_groundtruth(db,rows,cols);
		uint32_t * ml = weave_samples_wrapper(db,rows,cols);
		int64_t start,end;
		double cycles = 0.;
		
		uint32_t* results = malloc(rows * sizeof(uint32_t));
		uint32_t *temps = malloc(rows * sizeof(uint32_t));
		for(size_t i = 0 ; i < rows; ++i ){
				results[i]  = 0;
				temps[i] = 0;
		}
		size_t numEntries = numberOfEntries(rows,cols);
		
		/// Warmup
		
		start = start_tsc();
		for(size_t i = 0; i < N_WARMUP || end * i < 1e8; ++i){
			
			
			q(ml,results,temps,32,512,rows,cols,numEntries);

			correct = correct && compare(results,gt,rows);
			end = stop_tsc( start);
		}
		
		if(correct)
			printf("Warmup correct: TRUE \n");
		else
			printf("Warmup correct: FALSE \n");
		//calculation

		start = start_tsc();
    for(size_t i = 0; i < N_PERFORMANCE_INTERATION; ++i){
			q(ml,results,temps,32,512,rows,cols,numEntries);
    }
		end = stop_tsc(start);
		correct = correct && compare(results,gt,rows);
		if(correct)
			printf("Run correct: TRUE \n");
		else
			printf("Run correct: FALSE \n");

		free(db);
		free(gt);
		free(ml);
		free(results);
		free(temps);
		
		cycles = ((double)end) / N_PERFORMANCE_INTERATION;

    return cycles;
}


double perf_test_q2(q2_t q,generator gen,size_t rows,size_t cols){
    bool correct = true;
		int64_t start,end;
		double cycles = 0.;
		uint32_t * db = generateDB(rows,cols,gen);
		uint64_t  gt =  q2_groundtruth(db,rows,cols);
		uint32_t * ml = weave_samples_wrapper(db,rows,cols);


    uint32_t samples_per_block = 512 / cols;
    uint32_t * cond_buffer = malloc(samples_per_block * sizeof(uint32_t));
    uint32_t * temp_buffer = malloc(samples_per_block * sizeof(uint32_t));
    uint32_t * sum_buffer = malloc(samples_per_block * sizeof(uint32_t));
    size_t numEntries = numberOfEntries(rows,cols);
		uint64_t res;
		
		/// Warmup
		
		start = start_tsc();
		for(size_t i = 0; i < N_WARMUP || end * i < 1e8; ++i){
				memset(cond_buffer,0,samples_per_block*4);
				memset(temp_buffer,0,samples_per_block*4);
				memset(sum_buffer,0,samples_per_block*4);
				res = q(ml,cond_buffer,temp_buffer,sum_buffer,32,512,rows,cols,numEntries);
        correct = correct && res == gt;
				end = stop_tsc( start);
		}
		
		if(correct)
			printf("Warmup correct: TRUE \n");
		else
			printf("Warmup correct: FALSE \n");
    
		//calculation

		start = start_tsc();
    for(size_t i = 0; i < N_PERFORMANCE_INTERATION; ++i){
				memset(cond_buffer,0,samples_per_block*4);
				memset(temp_buffer,0,samples_per_block*4);
				memset(sum_buffer,0,samples_per_block*4);
				res = q(ml,cond_buffer,temp_buffer,sum_buffer,32,512,rows,cols,numEntries);
    }
		end = stop_tsc(start);
		correct = correct && res == gt;
		if(correct)
			printf("Run correct: TRUE \n");
		else
			printf("Run correct: FALSE \n");

    free(cond_buffer);
    free(temp_buffer);
    free(sum_buffer);
		free(db);
		free(ml);
		cycles = ((double)end) / N_PERFORMANCE_INTERATION;

    return cycles;
}

//
//bool test_q3(q3_t q, generator R_gen,generator S_gen,size_t R_rows,size_t R_cols, size_t S_rows,size_t S_cols){
//    bool correct = true;
//    for(size_t i = 0; i < N_RUNS;++i){
//        uint32_t* R = generateDB(R_rows,R_cols,R_gen);
//        uint32_t* S = generateDB(S_rows,S_cols,S_gen);
//        uint32_t * R_weave = weave_samples_wrapper(R,R_rows,R_cols);
//        uint32_t * S_weave = weave_samples_wrapper(S,S_rows,S_cols);
//
//        // PRINT_MALLOC(R,R_rows,R_cols);
//        uint32_t gt_out_size = cartesian_product_size(R_rows,R_cols,S_rows,S_cols);
//
//        size_t out_cols = R_cols + S_cols;
//
//        uint32_t* gt = (uint32_t*) malloc(gt_out_size * sizeof(uint32_t));
//        q3(R,S,gt,&gt_out_size,R_rows,R_cols,S_rows,S_cols);
//        free(R);
//        free(S);
//        uint32_t* re_gt = realloc(gt,gt_out_size * out_cols *  sizeof(uint32_t));
//
//
//        
//
//        uint32_t comp_out_size = cartesian_product_size(R_rows,R_cols,S_rows,S_cols);
//        uint32_t* comp = (uint32_t*) malloc(comp_out_size * sizeof(uint32_t));
//        uint32_t* R_buffer = (uint32_t*) malloc(R_cols * sizeof(uint32_t));
//        uint32_t* S_buffer = (uint32_t*) malloc(S_cols * sizeof(uint32_t)); 
//        q(R_weave,S_weave,comp,&comp_out_size,R_buffer,S_buffer,R_rows,R_cols,S_rows,S_cols,32,16);
//
//        uint32_t* re_comp = realloc(comp,comp_out_size * out_cols *  sizeof(uint32_t));
//
//        correct = correct && compare_rows_cols(gt,comp,MAX(comp_out_size,gt_out_size),out_cols);
//        correct = correct && gt_out_size == comp_out_size;
//        
//        
//        // printf("Correct: %d => Real_size=%d, got_size=%d\n",correct,gt_out_size,comp_out_size);
//
//
//        // PRINT_MALLOC(re_gt,gt_out_size,out_cols);
//        // PRINT_MALLOC(re_gt,gt_out_size,out_cols);
//
//        free(R_weave);
//        free(S_weave);
//        free(R_buffer);
//        free(S_buffer);
//        free(re_comp);
//        free(re_gt);
//    }
//    return correct;
//}
//
//
//
//uint64_t q2_wrapper(q2_t q,uint32_t* data,size_t rows,size_t cols){
//    uint32_t samples_per_block = 512 / cols;
//    uint32_t * cond_buffer = malloc(samples_per_block * sizeof(uint32_t));
//    uint32_t * temp_buffer = malloc(samples_per_block * sizeof(uint32_t));
//    uint32_t * sum_buffer = malloc(samples_per_block * sizeof(uint32_t));
//    memset(cond_buffer,0,samples_per_block*4);
//	memset(temp_buffer,0,samples_per_block*4);
//	memset(sum_buffer,0,samples_per_block*4);
//    size_t numEntries = numberOfEntries(rows,cols);
//    uint64_t res = q(data,cond_buffer,temp_buffer,sum_buffer,32,512,rows,cols,numEntries);
//    free(cond_buffer);
//    free(temp_buffer);
//    free(sum_buffer);
//    return res;
//}
//
//
//// void q3_wrapper(q3_t q,uint32_t *data,size_t rows,size_t cols){
////     uint32_t 
//// }
//
//bool compare(uint32_t * x, uint32_t *y,size_t n){
//    bool res = true;
//    for(int i = 0; i < n; ++i){
//        res = res && (x[i] == y[i]);
//    }
//    return res;
//}
//
//
//
//
//
//bool compare_rows_cols(uint32_t * x, uint32_t *y,size_t rows,size_t cols){
//    bool res = true;
//    printf("Got here\n");
//    for(int i = 0; i < rows; ++i){
//        // bool row_res = true;
//        for(int j = 0; j < cols;++j){
//            uint32_t xij = x[i * cols + j];
//            uint32_t yij =  y[i * cols + j];
//            res = res && xij == yij;
//            // row_res = row_res && xij == yij;
//
//        }
//        if(true){
//            printf("[%d] =>",i);
//            for(int j = 0; j < cols; ++j){
//                uint32_t xij = x[i * cols + j];
//                uint32_t yij =  y[i * cols + j];
//                printf(" %u ",xij);
//            }
//            for(int j = 0; j < cols; ++j){
//                uint32_t xij = x[i * cols + j];
//                uint32_t yij =  y[i * cols + j];
//                if(xij == yij){
//                    printf(GRN " %u " RESET ,yij);
//                }else{
//                    printf(RED " %u " RESET ,yij);
//                }
//                    
//            }
//            printf("\n");
//            
//        }
//    }
//    return res;
//
//}
//


void saveCycledataToFile( char* filename,size_t cycles, size_t rows, size_t cols, size_t gen){

   FILE *fptr;
   fptr = fopen(filename,"a");
   if(fptr == NULL)
   {
      printf("ERROR: cannot write to file!\n");   
   }
   fprintf(fptr,"%d, %d, %d, %d\n",cycles,rows,cols,gen);
   fclose(fptr);
}
