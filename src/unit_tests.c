#include<stdbool.h>  
#include <assert.h>
#include <stdint.h> 
#include <immintrin.h>
#include <x86intrin.h>
#include "../include/generator.h"
#include "../include/unit_tests.h"
#include "../include/debug.h"

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

//THIS IS BORROWED FROM AGNER AND FOG (THANKS BOYS!)
//bsrq for 8 byte word and bsrl for 4 byte word

static inline uint32_t bit_scan_reverse(uint32_t a) { // this computes log2(a)
    uint32_t r;
    __asm("bsrl %1, %0" : "=r"(r) : "r"(a) : );
    return r;
}


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
	uint32_t sh1,sh2,t,res;
	// L = 
	int L = bit_scan_reverse(d - 1) + 1;
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
	L = bit_scan_reverse(d - 1) + 1;
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


	L = bit_scan_reverse(d - 1) + 1;
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

	L = bit_scan_reverse(d - 1) + 1;
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



uint32_t fast_mod(uint32_t x, uint32_t y){
	return x % y;
}


void test_fast_uint_mod(size_t N){
	uint32_t res;
	size_t count = 0;
	size_t count_corr = 0;
	for(size_t i = 0; i < N;++i){
		for(size_t j = 3; j < N;++j){
			// res = fast_uint_div(i,j);
			uint32_t x = rand_gen(0,0);
			uint32_t y = rand_gen(0,0) + 1;
			res = fast_mod(x,y);
			bool correct = x % y == res;
			if(!correct){
				printf(RED "FAILED" RESET);
				printf("%u/%u = %u, but got %u\n",x,y,(x%y),res);
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




// uint32_t uint_sub_for_vector(uint32_t x , uint32_t y){
	
	
// 	return 
// }