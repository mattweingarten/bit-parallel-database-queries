#include<stdbool.h>  
#include <assert.h>
#include <stdint.h> 
#include <immintrin.h>
#include <x86intrin.h>
// #include "../include/tsc_x86.h"
#include "../include/generator.h"
#include "../include/unit_tests.h"
#include "../include/validate.h"
#include "../include/debug.h"
#include "../include/converter.h"
#include "../include/perform.h"
#define min(a, b) (((a) < (b)) ? (a) : (b))

uint32_t lt(uint32_t x, uint32_t y){
	uint32_t res = 0;
	uint32_t temp = 0;
	for(int i = 31; i >=0;i--){
		uint32_t xi = (x >> i) &1;
		uint32_t yi = (y >> i) &1;
		uint32_t xor = xi ^ yi;
		res = res | ((xor & yi) & (!temp));
		temp = temp | (xor & xi);
	}
	return res;
}

void test_lt(void){
    for(uint32_t i = 0;i < 1L << 23;i++){
		uint32_t x = (uint32_t) rand();
		uint32_t y = (uint32_t) rand();
		assert(lt(x,y) == x < y && "LT test did not pass!");
	}
    printf("Test lt passed!\n");
}



// * Mathematical formula, used for unsigned division with variable divisor:
// * (Also from T. Granlund and P. L. Montgomery)
// * x = dividend
// * d = divisor
// * w = integer word size, bits
// * L = ceil(log2(d)) = bit_scan_reverse(d-1)+1
// * m = 1 + 2^w * (2^L-d) / d                      [2^L should overflow to 0 if L = w]
// * sh1 = min(L,1)
// * sh2 = max(L-1,0)
// * t = m*x >> w                                   [high part of unsigned multiplication with 2w bits]
// * result = floor(x/d) = (((x-t) >> sh1) + t) >> sh2



// static inline uint32_t bit_scan_reverse(uint64_t a) {
//     uint64_t r;
//     __asm("bsrq %1, %0" : "=r"(r) : "r"(a) : );
//     return uint32_t(r);
// }


// static  void set(uint32_t d) {                                 // Set or change divisor, calculate parameters
//     uint32_t L, L2, sh1, sh2, m;
// 	switch (d) {
// 	case 0:
// 		m = sh1 = sh2 = 1 / d;                         // provoke error for d = 0
// 		break;
// 	case 1:
// 		m = 1; sh1 = sh2 = 0;                          // parameters for d = 1
// 		break;
// 	case 2:
// 		m = 1; sh1 = 1; sh2 = 0;                       // parameters for d = 2
// 		break;
// 	default:                                           // general case for d > 2
// 		L = bit_scan_reverse(d - 1) + 1;               // ceil(log2(d))
// 		L2 = uint32_t(L < 32 ? 1 << L : 0);            // 2^L, overflow to 0 if L = 32
// 		m = 1 + uint32_t((uint64_t(L2 - d) << 32) / d); // multiplier
// 		sh1 = 1;  sh2 = L - 1;                         // shift counts
// 	}
// }


uint32_t fast_uint_div_v2(uint32_t dividend, uint32_t d){
	uint32_t sh1,sh2,t,res,L;
	// L = 
	__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );
	L += 1;
	uint64_t L2 = 1L << L;
	uint64_t  N2 = 1L << 31;
	uint64_t m_full = floor(N2 * (L2 - d) / d) + 1;
	uint32_t m_low = (uint32_t) m_full;
	sh1 = min(L,1);
	sh2 = min(L - 1,0);
	uint64_t t1 = m_low * dividend;
	t = t1 >> 32;
	res = (((dividend - t) >> sh1) + t) >> sh2;
	return res;
}


uint32_t fast_uint_div(uint32_t dividend, uint32_t d){
	// LINE;LINE;LINE;
	uint32_t L,L2,m,sh1,sh2;
	uint32_t t;
	uint32_t res;
	__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );
	L += 1;
	// uint32_t L_test = log()
	// assert(L < 32);
	// L2 = 1 << L;
	// m = floor( (1L << 31) * (L2 - d ) /d) + 1;
	// sh1 = min(L,1);
	// sh2 = min(L - 1,0);
	// uint64_t t1 = m * dividend;
	// t = t1 >> 32;
	// // assert(L == )
	// L2 = (uint32_t) (L < 32? 1 << L : 0);
	// L2 = 1L << L;
	// uint64_t temp = (L2 - d);
	// temp = temp << 32;
	// // PRINT_64_B(temp);LINE;
	// temp = temp / d;
	// // PRINT_64_B(temp);LINE;
	// // printf("%u - %u / %u = %lu\n",L2,d,d,temp);
	// m = 1 + temp;
	// // PRINT_32_B(m);LINE;
	// sh1 = 1; sh2 = L-1;
	// sh1 = min(L,1); sh2 = min(L-1,0);
	// printf("")
	// PRINT_32_B(m);LINE;


	__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );
	L += 1;
	L2 = (uint32_t) (L < 32? 1 << L : 0);
	m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
	sh1 = 1; sh2 = L-1;



	uint64_t t1 =(uint64_t) m * (uint64_t) dividend;
	// PRINT_64_B(t1);LINE;
	// printf("%u * %u = %lu\n",m,dividend,t1);
	// debug_printf
	t = t1 >> 32; 
	// t = 
	res = (((dividend - t) >> sh1) + t) >> sh2;

	// printf("L:%u|L2:%u|m:%u|sh1:%u|sh2:%u|t:%u|temp:%lu\n",L,L2,m,sh1,sh2,t,temp);
	// printf("L:%u|L2:%u|m:%u|sh1:%u|sh2:%u|t:%u\n",L,L2,m,sh1,sh2,t);
	// // t = (uint64_t) 
	// printf("RES: %u\n",res);
	return res;

}

void test_fast_uint_div(size_t N){
	uint32_t res;
	size_t count = 0;
	size_t count_corr = 0;
	for(size_t i = 0; i < N;++i){
		for(size_t j = 3; j < N;++j){
			// res = fast_uint_div(i,j);
			uint32_t x = rand_gen(0,0);
			uint32_t y = rand_gen(0,0) + 1;
			res = fast_uint_div(x,y);
			bool correct = x / y == res;
			if(!correct){
				printf(RED "FAILED" RESET);
				printf("%u/%u = %u, but got %u\n",x,y,(x/y),res);
			}else{
				count_corr++;
				// printf(GRN "PASSED" RESET);
				// printf("%u/%u = %u\n",i,j,res);
			}
			count++;
			// assert(((i / j)  == fast_uint_div(i,j)));
		}
	}

	printf("Finished test: %u/%u correct \n",count_corr,count);
}



void fast_integer_division(uint32_t * x,uint32_t d,  uint32_t * dest,size_t N){
	uint32_t L,L2,m,sh1,sh2;

	__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );
	L += 1;
	L2 = (uint32_t) (L < 32? 1 << L : 0);
	m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
	sh1 = 1; sh2 = L-1;


	__m256i m_v = _mm256_set1_epi32(m);

	for(size_t i = 0; i < N ; i+= 8){
		__m256i x_v =  _mm256_loadu_si256(x + i);

		__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
		t_lo = _mm256_srli_epi64(t_lo,32);
		__m256i t_hi = _mm256_srli_epi64(x_v,32);
		t_hi = _mm256_mul_epu32(t_hi,m_v);
		__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
		__m256i res = _mm256_sub_epi32(x_v,t);
		res = _mm256_srli_epi32(res,sh1);
		res = _mm256_add_epi32(res,t);
		res = _mm256_srli_epi32(res,sh2);
		_mm256_storeu_si256(dest + i,res);

	}
}

void test_integer_vector_division(size_t N){
	assert(N % 8 == 0 && "Require N to be divisible by 8!");
	uint32_t* input1 = aligned_alloc(64,N * sizeof(uint32_t));
	uint32_t* dest = aligned_alloc(64,N * sizeof(uint32_t));
	for(size_t i = 0;i < N;++i ){
		input1[i] = rand_gen(0,0);
	}


	bool overall_correct = true;
	bool correct;
	for(size_t k = 0; k < N; ++k){
		uint32_t divisor = rand_gen(0,0) + 2; //divisior > 1
		fast_integer_division(input1,divisor,dest,N);
		for(size_t i = 0;i < N;++i ){
			correct = input1[i] / divisor == dest[i];
			if(!correct){
				overall_correct = false;
				printf(RED "FAILED" RESET);
				printf("%u/%u = %u, but got %u\n",input1[i],divisor,input1[i] / divisor,dest[i]);
			}
		}
	}



	if(overall_correct){
		printf(GRN "PASSED" RESET);printf(" INTEGER DIVSION TEST\n");
	}




	free(input1);
	free(dest);
	
}



void test_integer_vector_mod(size_t N){
	assert(N % 8 == 0 && "Require N to be divisible by 8!");
	uint32_t* input1 = aligned_alloc(64,N * sizeof(uint32_t));
	uint32_t* dest = aligned_alloc(64,N * sizeof(uint32_t));
	for(size_t i = 0;i < N;++i ){
		input1[i] = rand_100_gen(0,0);
		// input1[i] = rang_gen(0,0);
	}


	bool overall_correct = true;
	bool correct;
	for(size_t k = 0; k < N; ++k){
		// uint32_t divisor = rand_gen(0,0) + 2; //divisior > 1
		uint32_t divisor = rand_100_gen(0,0) + 2; //divisior > 1
		// fast_integer_division(input1,divisor,dest,N);
		// fast_integer_mod(input1,divisor,dest,N);
		fast_integer_mod2(input1,divisor,dest,N);
		for(size_t i = 0;i < N;++i ){
			correct = input1[i] % divisor == dest[i];
			if(!correct){
				overall_correct = false;
				printf(RED "FAILED" RESET);
				printf(" %u mod %u = %u, but got %u\n",input1[i],divisor,input1[i] % divisor,dest[i]);
			}
		}
	}



	if(overall_correct){
		printf(GRN "PASSED" RESET);printf(" INTEGER MODULO TEST\n");
	}




	free(input1);
	free(dest);
	
}

void fast_integer_mod(uint32_t * x,uint32_t d,  uint32_t * dest,size_t N){
	uint32_t L,L2,m,sh1,sh2;

	__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );
	L += 1;
	L2 = (uint32_t) (L < 32? 1 << L : 0);
	m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
	sh1 = 1; sh2 = L-1;


	__m256i m_v = _mm256_set1_epi32(m);
	__m256i d_v = _mm256_set1_epi32(d);
	for(size_t i = 0; i < N ; i+= 8){
		__m256i x_v =  _mm256_loadu_si256(x + i);

		__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
		t_lo = _mm256_srli_epi64(t_lo,32);
		__m256i t_hi = _mm256_srli_epi64(x_v,32);
		t_hi = _mm256_mul_epu32(t_hi,m_v);
		__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
		__m256i res = _mm256_sub_epi32(x_v,t);
		res = _mm256_srli_epi32(res,1);
		res = _mm256_add_epi32(res,t);
		res = _mm256_srli_epi32(res,sh2);
		res = _mm256_mullo_epi32(res,d_v);
		res = _mm256_sub_epi32(x_v,res);
		_mm256_storeu_si256(dest + i,res);

	}


}
void fast_integer_mod2(uint32_t * x,uint32_t d,  uint32_t * dest,size_t N){


	__m256i d_v = _mm256_set1_epi32(d);
	__m256 d_v_f = _mm256_cvtepi32_ps(d_v);
	for(size_t i = 0; i < N ; i+= 8){
		__m256i x_v =  _mm256_loadu_si256(x + i);
		__m256 x_v_f = _mm256_cvtepi32_ps(x_v);
		__m256 division = _mm256_div_ps(x_v_f,d_v_f);
		__m256 rounded = _mm256_floor_ps(division);
		__m256 remainder = _mm256_sub_ps(division,rounded);
		__m256 result = _mm256_mul_ps(remainder,d_v_f);
		__m256i res_uint = _mm256_cvtps_epi32(result);
		_mm256_storeu_si256(dest + i,res_uint);
	}



}




void base_line_integer_mod(uint32_t * x,uint32_t d,  uint32_t * dest,size_t N){
	for(size_t i = 0; i < N;++i){
		dest[i] = x[i] % d;
	}
}


void test_fast_recon(size_t rows,size_t cols, generator gen){
	assert(rows * cols  == 512 && "just testing 1");
	uint32_t * db = generateDB(rows,cols,gen);
	uint32_t * ml = weave_samples_wrapper(db,rows,cols);
	uint32_t * res = (uint32_t * ) malloc(512 * sizeof(uint32_t));
	// PRINT_MALLOC(db,rows,cols);
	// HLINE;

	// PRINT_MALLOC_H(db,rows * cols);
	// PRINT_MALLOC(db,rows,cols);
	// HLINE;


	fast_recon(ml,res,rows,cols);
	// PRINT_MALLOC(res,rows,1);
	// __m256i x = 
	// PRINT_32_BIT_VECTOR(x);
	
	// PRINT_WEAVED(ml,rows,cols);
	uint64_t start,end;
	start = start_tsc();
	for(int i = 0; i < 1000;++i){
		// fast_recon_v2(ml,res,rows,cols);
		fast_recon(ml,res,rows,cols);
	}
	end = stop_tsc(start);



	// start = start_tsc();
	// for(int i = 0; i < 1000;++i){
	// 	// fast_recon_v2(ml,res,rows,cols);
	// 	fast_recon(ml,res,rows,cols);
	// }
	// end = stop_tsc(start);
	// double cycles = ((double) end ) / 1000;
	// printf("Cycles : %lf\n",cycles);
	
	// bool correct = compare(res,db,)
	// free(db);
	// free(ml);
	// free(res);

}



void fast_recon(uint32_t * src, uint32_t *dest,size_t rows, size_t cols){
	size_t wordsize= 32;
	size_t cl_size = 16;
	size_t smpls_per_entry = 32 / cols;
	uint32_t * _1bit_index = src;
	uint32_t * _2bit_index = src + cl_size * 8;
	uint32_t * _3bit_index = src + cl_size * 16;
	uint32_t * _4bit_Index = src + cl_size * 24;
	// PRINT_WEAVED(src,rows,cols);
	__m256i mask = _mm256_set1_epi32(1);
	__m256i x_07,x_815,x_1623,x_2432,x_07_v2,x_815_v2,x_1623_v2,x_2432_v2;
	__m256i shift_index_07 = _mm256_set_epi32(0,1,2,3,4,5,6,7);
	__m256i shift_index_815 = _mm256_set_epi32(8,9,10,11,12,13,14,15);
	__m256i shift_index_1623 = _mm256_set_epi32(16,17,18,19,20,21,22,23);
	__m256i shift_index_2432 = _mm256_set_epi32(24,25,26,27,28,29,30,31);
	__m256i index = _mm256_setr_epi32(0,16,32,48,64,80,96,112);
	size_t dest_index = 0;
	for(size_t  j = 0; j < cl_size;++j){
		x_07 = _mm256_i32gather_epi32(_1bit_index,index,4);
		x_815 = _mm256_i32gather_epi32(_2bit_index,index,4);
		x_1623 = _mm256_i32gather_epi32(_3bit_index,index,4);
		x_2432 = _mm256_i32gather_epi32(_4bit_Index,index,4);
		for(size_t k = 0; k < 32;k+=cols){

			x_07_v2 = _mm256_srli_epi32(x_07, k );
			x_815_v2 = _mm256_srli_epi32(x_815, k );
			x_1623_v2 = _mm256_srli_epi32(x_1623_v2, k );
			x_2432_v2 = _mm256_srli_epi32(x_2432, k );

			x_07_v2 = _mm256_and_si256(x_07_v2,mask);
			x_815_v2 = _mm256_and_si256(x_815_v2,mask);
			x_1623_v2 = _mm256_and_si256(x_1623_v2,mask);
			x_2432_v2 = _mm256_and_si256(x_2432_v2,mask);

			x_07_v2 = _mm256_sllv_epi32(x_07_v2,shift_index_2432);
			x_815_v2 = _mm256_sllv_epi32(x_815,shift_index_1623);
			x_1623_v2 = _mm256_sllv_epi32(x_1623_v2,shift_index_815);
			x_2432_v2 = _mm256_sllv_epi32(x_2432_v2,shift_index_07);

			__m256i x_0_15 = _mm256_or_si256  (x_07_v2,x_815_v2);
			__m256i x_16_32 = _mm256_or_si256  (x_1623_v2,x_2432_v2);
			__m256i x_all = _mm256_or_si256  (x_0_15,x_16_32);
			uint32_t* ptr = (uint32_t*) &x_all;
			uint32_t res = 0;
			for(size_t m = 0;m < 8;++m){
				res += ptr[m];
			}

			dest[dest_index] = res;
			dest_index++;
			

		}
		_1bit_index++;
		_2bit_index++;
		_3bit_index++;
		_4bit_Index++;
	}
}

	void fast_recon_v2(uint32_t * src, uint32_t *dest,size_t rows, size_t cols){
		size_t wordsize= 32;
		size_t cl_size = 16;
		size_t smpls_per_entry = 32 / cols;
		size_t R_buffer_size = smpls_per_entry * cl_size;

		// for(size_t i = 0; i < R_num_cl_blocks; ++i){
		// start1 = start_tsc();
		// memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
		for(size_t k  = 0; k < wordsize;++k){
			for(size_t m = 0; m < cl_size; ++m){
				uint32_t next_word = src[k * cl_size + m];
				
				for (size_t n = 0; n < smpls_per_entry;++n){
					uint32_t k_th_bit_R_a = (next_word >> (n * cols)) & 1;
					uint32_t res = (k_th_bit_R_a << (wordsize - k - 1));
					// R_a_buffer[m * R_samples_per_entry + n] += (k_th_bit_R_a << (wordsize - k - 1));
				}
			}
		}

	}



void perf_mod(size_t N){
	assert(N % 8 == 0 && "Require N to be divisible by 8!");
	uint32_t* input1 = aligned_alloc(64,N * sizeof(uint32_t));
	uint32_t* dest = aligned_alloc(64,N * sizeof(uint32_t));

	for(size_t i = 0;i < N;++i ){
		input1[i] = rand_gen(0,0);
		
	}
	uint64_t start,end;
	start = start_tsc();
	uint32_t divisor = rand_gen(0,0) + 2;
	for(size_t i = 0; i < N_WARMUP;++i){
		fast_integer_mod(input1,divisor,dest,N);
	}

	start = start_tsc();
	for(size_t i = 0; i <N_PERF_ITERATION;++i){
		fast_integer_mod(input1,divisor,dest,N);
	}

	end = stop_tsc(start);
	free(input1);
	free(dest);
	// uint64_t cycles = end /N_PERF_ITERATION; 
	uint64_t cycles = end /N_PERF_ITERATION; 
	printf("%lu,%u\n",cycles,N);
}




void perf_mod_2(size_t N){
	assert(N % 8 == 0 && "Require N to be divisible by 8!");
	uint32_t* input1 = aligned_alloc(64,N * sizeof(uint32_t));
	uint32_t* dest = aligned_alloc(64,N * sizeof(uint32_t));

	for(size_t i = 0;i < N;++i ){
		input1[i] = rand_gen(0,0);
		
	}
	uint64_t start,end;
	start = start_tsc();
	// uint32_t divisor = 19184124;
	uint32_t divisor = rand_gen(0,0) + 2;
	for(size_t i = 0; i < N_WARMUP;++i){
		base_line_integer_mod(input1,divisor,dest,N);
		end = stop_tsc( start);
	}

	start = start_tsc();
	
	for(size_t i = 0; i <N_PERF_ITERATION;++i){
		base_line_integer_mod(input1,divisor,dest,N);
	}
	end = stop_tsc(start);
	// printf("Start: %lu,end:%lu",start,end);
	free(input1);
	free(dest);
	double cycles = ((double)end) / N_PERF_ITERATION; 
	printf("%lf,%u\n",cycles,N);
}



void perf_mod_3(size_t N){
	assert(N % 8 == 0 && "Require N to be divisible by 8!");
	uint32_t* input1 = aligned_alloc(64,N * sizeof(uint32_t));
	uint32_t* dest = aligned_alloc(64,N * sizeof(uint32_t));

	for(size_t i = 0;i < N;++i ){
		input1[i] = rand_gen(0,0);
		
	}
	uint64_t start,end;
	start = start_tsc();
	// uint32_t divisor = 19184124;
	uint32_t divisor = rand_gen(0,0) + 2;
	for(size_t i = 0; i < N_WARMUP;++i){
		fast_integer_mod2(input1,divisor,dest,N);
		end = stop_tsc( start);
	}

	start = start_tsc();
	
	for(size_t i = 0; i <N_PERF_ITERATION;++i){
		fast_integer_mod2(input1,divisor,dest,N);
	}
	end = stop_tsc(start);
	// printf("Start: %lu,end:%lu",start,end);
	free(input1);
	free(dest);
	double cycles = ((double)end) / N_PERF_ITERATION; 
	printf("%lf,%u\n",cycles,N);
}




void take_perf(size_t maxN,size_t steps){
	size_t n = maxN/steps;
	assert((maxN/steps)%8 == 0);
	for(size_t i = n; i < maxN;i+=n ){
		perf_mod(i);
	}
	for(size_t i = n; i < maxN;i+=n ){
		perf_mod_2(i);
	}

	for(size_t i = n; i < maxN;i+=n ){
		perf_mod_3(i);
	}
}