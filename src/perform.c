

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

void performance_rnd_query(void* query, enum Query type,char * out_file_name){
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
		
		uint32_t* results = aligned_alloc( 32, rows * sizeof(uint32_t));
		uint32_t *temps = aligned_alloc( 32, rows * sizeof(uint32_t));
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
		for(size_t i = 0; i < N_PERF_ITERATION; ++i){
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
		
		cycles = ((double)end) / N_PERF_ITERATION;

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
    uint32_t * cond_buffer = aligned_alloc( 32, samples_per_block * sizeof(uint32_t));
    uint32_t * temp_buffer = aligned_alloc( 32, samples_per_block * sizeof(uint32_t));
    uint32_t * sum_buffer = aligned_alloc( 32, samples_per_block * sizeof(uint32_t));
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
    for(size_t i = 0; i < N_PERF_ITERATION; ++i){
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
	cycles = ((double)end) / N_PERF_ITERATION;

    return cycles;
}





double perf_test_q3(q3_t q,generator R_gen,generator S_gen,size_t R_rows,size_t R_cols, size_t S_rows,size_t S_cols){

	int64_t start,end;
	double cycles = 0.;
	uint32_t* R = generateDB(R_rows,R_cols,R_gen);
	uint32_t* S = generateDB(S_rows,S_cols,S_gen);
	uint32_t * R_weave = weave_samples_wrapper(R,R_rows,R_cols);
	uint32_t * S_weave = weave_samples_wrapper(S,S_rows,S_cols);
	free(R);
	free(S);
	uint32_t res_out_size = cart_prod(R_rows,S_rows);
	uint32_t* res = (uint32_t*) aligned_alloc( 32, res_out_size * 2 * sizeof(uint32_t));
	q(R_weave,S_weave,res,&res_out_size,R_rows,R_cols,S_rows,S_cols,32,16);
	uint32_t* re_res = realloc(res,res_out_size * 2 *  sizeof(uint32_t));
	
	// Warmup
	start = start_tsc();
	for(size_t i = 0; i < N_WARMUP || end * i < 1e8; ++i){
		q(R_weave,S_weave,re_res,&res_out_size,R_rows,R_cols,S_rows,S_cols,32,16);
		end = stop_tsc(start);
	}


	//calculation

	start = start_tsc();
	for(size_t i = 0; i < N_PERF_ITERATION; ++i){
		// printf("Calulating %d\n",i);
		q(R_weave,S_weave,re_res,&res_out_size,R_rows,R_cols,S_rows,S_cols,32,16);
	}

	end = stop_tsc(start);

	free(R_weave);
	free(S_weave);
	free(re_res);
	
	return ((double)end) / N_PERF_ITERATION;
}



double perf_test_q3_blocked(q3b_t q,generator R_gen,generator S_gen,size_t R_rows,size_t R_cols, size_t S_rows,size_t S_cols,size_t block_size){

	int64_t start,end;
	double cycles = 0.;
	uint32_t* R = generateDB(R_rows,R_cols,R_gen);
	uint32_t* S = generateDB(S_rows,S_cols,S_gen);
	uint32_t * R_weave = weave_samples_wrapper(R,R_rows,R_cols);
	uint32_t * S_weave = weave_samples_wrapper(S,S_rows,S_cols);
	uint32_t gt_out_size = cart_prod(R_rows,S_rows);
	uint32_t* gt = (uint32_t*) aligned_alloc( 32, gt_out_size * 2 * sizeof(uint32_t));
	q3_index(R,S,gt,&gt_out_size,R_rows,R_cols,S_rows,S_cols);
	uint32_t* re_gt = realloc(gt,gt_out_size * 2 *  sizeof(uint32_t));

	free(R);
	free(S);
	
	uint32_t res_out_size = cart_prod(R_rows,S_rows);
	uint32_t* res = (uint32_t*) aligned_alloc( 32, res_out_size * 2 * sizeof(uint32_t));
	q(R_weave,S_weave,res,&res_out_size,R_rows,R_cols,S_rows,S_cols,32,16,block_size);
	uint32_t* re_res = realloc(res,res_out_size * 2 *  sizeof(uint32_t));
	
	// Warmup
	start = start_tsc();
	for(size_t i = 0; i < N_WARMUP || end * i < 1e8; ++i){
		q(R_weave,S_weave,re_res,&res_out_size,R_rows,R_cols,S_rows,S_cols,32,16,block_size);
		end = stop_tsc(start);
	}


	//calculation

	start = start_tsc();
	for(size_t i = 0; i < N_PERF_ITERATION; ++i){
		// printf("Calulating %d\n",i);
		q(R_weave,S_weave,re_res,&res_out_size,R_rows,R_cols,S_rows,S_cols,32,16,block_size);
	}


	
	end = stop_tsc(start);


	bool correct = compare_join(re_gt,re_res,gt_out_size,res_out_size);
	if(!correct){
		printf("Failed a test!\n");
	}

	free(R_weave);
	free(S_weave);
	free(re_res);
	free(re_gt);
	
	return ((double)end) / N_PERF_ITERATION;
}




void perf_q3_R_rows(char* filename, q3_t q,generator gen,size_t R_row_max, size_t step_size,size_t R_cols,size_t S_rows, size_t S_cols){
	for(size_t i = step_size; i <= R_row_max;i += step_size){
		double cycles = perf_test_q3(q,gen,gen,i,R_cols,S_rows,S_cols);
		saveCycledataToFile_q3(filename,cycles,i,R_cols,S_rows,S_cols);
		printf("Cycles for rows = %d => %lf\n",i,cycles);
	}
}


void perf_q3_rows(char* filename, q3_t q,generator gen,size_t row_max, size_t R_cols,size_t S_cols,size_t step_size){
	for(size_t i = step_size; i <= row_max;i += step_size){
		double cycles = perf_test_q3(q,gen,gen,i,R_cols,i,S_cols);
		saveCycledataToFile_v1(filename,cycles,i,R_cols,S_cols);
		printf("Cycles for rows = %d => %lf\n",i,cycles);
	}
}



void perf_q3_compare_block(char * filename, q3b_t q,size_t max_row_size){
	for(size_t i = 1024; i <= max_row_size; i *= 2){
		size_t max_block = (i * 4) /512;
		for(size_t block_size = 1; block_size < max_block;++block_size){
			if(max_block % block_size == 0){

				double cycles = perf_test_q3_blocked(q,rand_gen,rand_100_gen,i,4,i,4,block_size);
				saveCycledataToFile_q3_block_cmp(filename,cycles,i,i,block_size);
				printf("Cycles for rows= %d blocks= %d => %lf\n",i,block_size,cycles);
			}
		}
	}
}

void saveCycledataToFile_q3( char* filename,double cycles, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols){

   char buf[1024];
   strcpy(buf, Q3_PATH );
   strcat(buf,filename);
   FILE *fptr;
   fptr = fopen(buf,"a");
   if(fptr == NULL)
   {
      printf("ERROR: cannot write to file!\n");   
   }
   fprintf(fptr,"%lf, %d, %d, %d, %d\n",cycles,R_rows,R_cols,S_rows,S_cols);
   fclose(fptr);
}

void saveCycledataToFile_q3_block_cmp( char* filename,size_t cycles, size_t R_rows, size_t S_rows,size_t block_size){

   char buf[1024];
   strcpy(buf, Q3_PATH );
   strcat(buf,filename);
   FILE *fptr;
   fptr = fopen(buf,"a");
   if(fptr == NULL)
   {
      printf("ERROR: cannot write to file!\n");   
   }
   fprintf(fptr,"%d, %d, %d, %d\n",cycles,R_rows,S_rows,block_size);
   fclose(fptr);
}

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



void saveCycledataToFile_v1( char* filename,double cycles, size_t rows, size_t R_cols, size_t S_cols){
   char buf[1024];
   strcpy(buf, Q3_PATH );
   strcat(buf,filename);
   FILE *fptr;
   fptr = fopen(buf,"a");
   if(fptr == NULL)
   {
      printf("ERROR: cannot write to file!\n");   
   }
   fprintf(fptr,"%lf, %d, %d, %d\n",cycles,rows,R_cols,S_cols);
   fclose(fptr);
}