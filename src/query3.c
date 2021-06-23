
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query3.h"
#include "../include/tsc_x86.h"

#include <immintrin.h>




void q3_index(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols){
	size_t dest_row = 0;
	size_t dest_col = 2;
	uint32_t R_a;
	uint32_t S_b;
	uint32_t S_c;
	for(size_t  i = 0; i < R_rows; i++){
		for(size_t j = 0 ; j < S_rows; j++){
			R_a = dR[i * R_cols + 0];
			S_b = dS[j * S_cols + 1];
			S_c = dS[j * S_cols + 2];
			if(S_b != 0 && R_a % S_b == S_c){
				dest[dest_row * dest_col] = i;
				dest[dest_row * dest_col  + 1] = j;
				dest_row++;
			}
		}
	}
	*dest_rows = dest_row;
}






void q3_weave_index(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
	



	size_t cl_block_size = wordsize * cl_size;

	size_t dest_index = 0;

	size_t R_cl_index;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_cl_index;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;
	


	uint32_t R_a;
	uint32_t S_b;
	uint32_t S_c;



	

	// 512 * 32 = 2 ^ 16 -> L1 cache 
	for(size_t i = 0; i < R_rows; ++i){

		R_a = 0;
		size_t R_cl_block_index = (i * R_cols) / cl_block_size;
		size_t R_cl_index = (i % R_samples_per_cl) / R_samples_per_entry; 
		size_t R_shift_index = (i % R_samples_per_entry) * R_cols;


		for(size_t k = 0;k < wordsize; ++k){
			size_t R_i_index = R_cl_block_index * cl_block_size + k * cl_size + R_cl_index;
			uint32_t kth_bit_of_R_a = (dR[R_i_index] >> R_shift_index ) &1;
			R_a += kth_bit_of_R_a << (wordsize - k - 1);
	
		}

		for(size_t j = 0; j < S_rows;++j){
			S_b = 0;
			S_c = 0;
			size_t S_cl_block_index = (j * S_cols) / cl_block_size;
			size_t S_cl_index = (j % S_samples_per_cl) / S_samples_per_entry; 
			size_t S_shift_index = (j % S_samples_per_entry) * S_cols;

			for(size_t l = 0; l < wordsize; ++l){

				size_t S_j_index = S_cl_block_index * cl_size * wordsize + l * cl_size + S_cl_index;

				uint32_t lth_bit_of_S_b = (dS[S_j_index] >> (S_shift_index + 1)) & 1;
				uint32_t lth_bit_of_S_c = (dS[S_j_index] >> (S_shift_index + 2)) & 1;
				S_b += (lth_bit_of_S_b << (wordsize - l - 1));
				S_c += (lth_bit_of_S_c << (wordsize - l - 1));
			}

			if(S_b != 0 && R_a % S_b == S_c){
				dest[dest_index * 2] = i;
				dest[dest_index * 2 + 1] = j;
				dest_index++;
			}
		}
	}
	*dest_rows = dest_index;
}






void q3_weave_index_l1_block(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){


	#ifdef _PROFILE
	myInt64 total_time;
	myInt64 total_time_res;
	myInt64 reconstruct_time;
	myInt64 reconstruct_time_res = 0;
	#endif
	size_t dest_index = 0;

	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;


	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	#ifdef _PROFILE
	total_time = start_tsc();
	#endif

	for(size_t i = 0; i < R_num_cl_blocks; ++i){
		#ifdef _PROFILE
		reconstruct_time = start_tsc();
		#endif
		memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
		for(size_t k  = 0; k < wordsize;++k){
			for(size_t m = 0; m < cl_size; ++m){
				uint32_t next_word = dR[i * cl_block_size + k * cl_size + m];
				
				for (size_t n = 0; n < R_samples_per_entry;++n){
					uint32_t k_th_bit_R_a = (next_word >> (n * R_cols)) & 1;
					R_a_buffer[m * R_samples_per_entry + n] += (k_th_bit_R_a << (wordsize - k - 1));
				}
			}
		}

		#ifdef _PROFILE
		myInt64 end = stop_tsc(reconstruct_time);
		reconstruct_time_res += end;
		#endif


		
		for(size_t j = 0; j < S_num_cl_blocks;++j){
			#ifdef _PROFILE
			reconstruct_time = start_tsc();
			#endif
			memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
			memset(S_c_buffer,0,S_smpls_per_cl_block * 4);
			for(size_t k  = 0 ; k < wordsize; ++k){
				for (size_t m = 0; m < cl_size; ++m){
					uint32_t next_word = dS[j * cl_block_size + k * cl_size + m];
					// printf("index : %d\n",i * cl_block_size + k * cl_size + m);
					if(i == 0){

					}
					for (size_t n = 0; n < S_samples_per_entry;++n){
			
						uint32_t k_th_bit_S_b = (next_word >> (n * S_cols + 1)) & 1;
						uint32_t k_th_bit_S_c = (next_word >> (n * S_cols + 2)) & 1;

						// TODO:  HERE WE WOULD ADD THE BIT BY BIT IMPLEMENTATION
						S_b_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_b << (wordsize - k - 1));
						S_c_buffer[m * S_samples_per_entry + n] += (k_th_bit_S_c << (wordsize - k - 1));
					}
				}
				
			}
				
			#ifdef _PROFILE
			myInt64 end = stop_tsc(reconstruct_time);
			reconstruct_time_res += end;
			#endif
			// HLINE;
			// PRINT_MALLOC(R_a_buffer,R_smpls_per_cl_block,1);
			// PRINT_MALLOC(R_a_buffer,R_smpls_per_cl_block,1);
			// PRINT_MALLOC(S_b_buffer,S_smpls_per_cl_block,1);
			// For now we do the actual join here:

			// start3 = start_tsc();
			for(size_t k = 0; k < R_smpls_per_cl_block; ++k){
				for(size_t l = 0; l < S_smpls_per_cl_block; ++l){
					if(S_b_buffer[l] != 0 && R_a_buffer[k] % S_b_buffer[l] == S_c_buffer[l]){
						dest[2 * dest_index] = i * R_smpls_per_cl_block + k; 
						dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + l;
						dest_index++;
					} 
				}
			}
			// end3 = stop_tsc(start3);
			// printf("%u %u %u\n",end1,end2,end3);
		}
	}
	#ifdef _PROFILE
	total_time_res = stop_tsc(total_time);
	printf("%lu,%lu\n",total_time_res,reconstruct_time_res);
	printf("%lf\n",100 * ((double) reconstruct_time_res)/ total_time_res);
	#endif
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	*dest_rows = dest_index;
	// printf("Cycles : %lf\n",cycles/msr_count);
}








void q3_vector_v5(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
	#ifdef _PROFILE
	myInt64 total_time;
	myInt64 total_time_res;
	myInt64 reconstruct_time;
	myInt64 reconstruct_time_res = 0;
	#endif


	size_t dest_index = 0;

	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;


	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));

	struct modular_operation* mod_ops  = (struct modular_operation*) aligned_alloc(32,S_smpls_per_cl_block * sizeof(struct modular_operation));


	uint32_t * S_next_word_index = dS;

	__m256i S_b_vector;
	__m256i S_c_vector;
	__m256i next_word;
		#ifdef _PROFILE
		total_time = start_tsc();
		#endif



	for(size_t j = 0; j < S_num_cl_blocks;++j){
		memset(S_b_buffer,0,S_smpls_per_cl_block * 4);
		memset(S_c_buffer,0,S_smpls_per_cl_block * 4);

		uint32_t word_shift_index = wordsize  - 1;
		#ifdef _PROFILE
		reconstruct_time = start_tsc();
		#endif
		for(size_t k  = 0 ; k < wordsize; ++k){


			for (size_t m = 0; m < cl_size; m += 8){


				next_word = _mm256_loadu_si256(S_next_word_index);
				uint32_t n_shift_b_index = 1;
				uint32_t n_shift_c_index = 2;
				uint32_t* S_b_buffer_index = S_b_buffer + m;
				uint32_t* S_c_buffer_index = S_c_buffer + m;
				for (size_t n = 0; n < S_samples_per_entry;++n){
	
					__m256i k_th_bit_S_b = _mm256_srli_epi32(next_word,n_shift_b_index);
					k_th_bit_S_b = _mm256_and_si256(k_th_bit_S_b,_mm256_set_epi32(1,1,1,1,1,1,1,1));
					k_th_bit_S_b = _mm256_slli_epi32(k_th_bit_S_b,word_shift_index);
					S_b_vector = _mm256_add_epi32(S_b_vector,k_th_bit_S_b);
					S_b_vector = _mm256_loadu_si256(S_b_buffer_index);
					S_b_vector = _mm256_add_epi32(S_b_vector,k_th_bit_S_b);
					_mm256_storeu_si256(S_b_buffer_index,S_b_vector);

	
					__m256i k_th_bit_S_c = _mm256_srli_epi32(next_word,n_shift_c_index);
					k_th_bit_S_c = _mm256_and_si256(k_th_bit_S_c,_mm256_set_epi32(1,1,1,1,1,1,1,1));
					k_th_bit_S_c = _mm256_slli_epi32(k_th_bit_S_c,word_shift_index);
					S_c_vector = _mm256_add_epi32(S_c_vector,k_th_bit_S_c);
					S_c_vector = _mm256_loadu_si256(S_c_buffer_index);
					S_c_vector = _mm256_add_epi32(S_c_vector,k_th_bit_S_c);
					_mm256_storeu_si256(S_c_buffer_index,S_c_vector);


					n_shift_b_index += S_cols;
					n_shift_c_index += S_cols;
					S_b_buffer_index += cl_size;
					S_c_buffer_index += cl_size;
					// printf("Sc buffer index : %u\n",S_b_buffer_index - S_b_buffer);
				}
				S_next_word_index += 8;
			}
			word_shift_index -= 1;
		}

		#ifdef _PROFILE
		myInt64 end = stop_tsc(reconstruct_time);
		reconstruct_time_res += end;
		#endif


		// PRINT_MALLOC(S_b_buffer,S_smpls_per_cl_block,1);LINE;
		// Loaded and rebuilt 1 cl_block of S
		// Now we can compute the values for fast modulo, the cost of this is amortized since we only do this 
		// ONCE for each row in S
		//TODO can we vectorize this part? how much can we gain from this?
		for(size_t k = 0; k < S_smpls_per_cl_block;++k){ 
			uint32_t L,L2;
			uint32_t d = S_b_buffer[k];
			switch(d){
				case 0: 
					
					continue; // here we just keep going and do not compute the values
					break;
				case 1:
					mod_ops[k].m = 1; mod_ops[k].sh1 = mod_ops[k].sh2 = 0;
					break;
				case 2:  
					mod_ops[k].m = 1; mod_ops[k].sh1 = 1; mod_ops[k].sh2 = 0;
					break;
				default:
					__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );  		//ceil(log2)
					L += 1;												//ceil(log2)
					L2 = (uint32_t) (L < 32? 1 << L : 0);
					mod_ops[k].m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
					mod_ops[k].sh1 = 1; mod_ops[k].sh2 = L-1;
				}

		}




	

		uint32_t * R_next_word_index = dR;
		for(size_t i = 0; i < R_num_cl_blocks; ++i){
		#ifdef _PROFILE
		reconstruct_time = start_tsc();
		#endif
		__m256i R_a_vector;
		__m256i next_word_R;
		memset(R_a_buffer,0,R_smpls_per_cl_block * 4);
			uint32_t word_shift_index = wordsize  - 1;
			for(size_t k  = 0; k < wordsize;++k){
				for(size_t m = 0; m < cl_size; m+= 8){ 

					uint32_t* R_a_buffer_index = R_a_buffer + m;

					next_word_R = _mm256_loadu_si256(R_next_word_index);
					uint32_t n_shift_index = 0;
					for (size_t n = 0; n < R_samples_per_entry;++n){
						__m256i k_th_bit_R_a = _mm256_srli_epi32(next_word_R,n_shift_index);
						k_th_bit_R_a = _mm256_and_si256(k_th_bit_R_a,_mm256_set_epi32(1,1,1,1,1,1,1,1)); 
						k_th_bit_R_a = _mm256_slli_epi32(k_th_bit_R_a,word_shift_index);
						R_a_vector = _mm256_loadu_si256(R_a_buffer_index);
						R_a_vector = _mm256_add_epi32(R_a_vector,k_th_bit_R_a);
						_mm256_store_si256(R_a_buffer_index,R_a_vector);
						

						n_shift_index += R_cols;
						R_a_buffer_index += cl_size;
					}
					R_next_word_index += 8;
				}
				word_shift_index -= 1;
			}

				
			#ifdef _PROFILE
			myInt64 end = stop_tsc(reconstruct_time);
			reconstruct_time_res += end;
			#endif


			for(size_t l = 0; l < S_smpls_per_cl_block;++l){
				struct modular_operation mod_l = mod_ops[l];
				uint32_t s_b = S_b_buffer[l];
				uint32_t s_c = S_c_buffer[l];


				__m256i m_v = _mm256_set1_epi32(mod_l.m);
				__m256i d_v = _mm256_set1_epi32(s_b);
				__m256i s_c_v = _mm256_set1_epi32(s_c);


				for(size_t k = 0; k < R_smpls_per_cl_block; k+= 8){
					__m256i x_v = _mm256_loadu_si256(R_a_buffer + k);
					__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
					t_lo = _mm256_srli_epi64(t_lo,32);
					__m256i t_hi = _mm256_srli_epi64(x_v,32);
					t_hi = _mm256_mul_epu32(t_hi,m_v);
					__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
					__m256i res = _mm256_sub_epi32(x_v,t);
					res = _mm256_srli_epi32(res,mod_l.sh1);
					res = _mm256_add_epi32(res,t);
					res = _mm256_srli_epi32(res,mod_l.sh2);
					res = _mm256_mullo_epi32(res,d_v);
					res = _mm256_sub_epi32(x_v,res);
					__m256i eq = _mm256_cmpeq_epi32(res,s_c_v);
					uint32_t* eq_vector = &eq;
		
					for(size_t h = 0; h < 8; ++h){
						if(eq_vector[h] == 4294967295L){
							size_t r_i = ((h + k) % cl_size) * R_samples_per_entry + ((h + k) / cl_size);
							size_t s_j = (l % cl_size) * S_samples_per_entry + (l / cl_size);
							dest[2 * dest_index] = i * R_smpls_per_cl_block + r_i;
							dest[2 * dest_index + 1] = j * S_smpls_per_cl_block + s_j;
							dest_index++;
						}
					}
				}

			}
		}// end cl_block R

	} //end cl_block S


	#ifdef _PROFILE
	total_time_res = stop_tsc(total_time);
	printf("%lu,%lu\n",total_time_res,reconstruct_time_res);
	printf("%lf\n",100 * ((double) reconstruct_time_res)/ total_time_res);	
	#endif
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	free(mod_ops);
	*dest_rows = dest_index;


}

void q3_fast_recon_fast_modulo(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
	#ifdef _PROFILE
	myInt64 total_time;
	myInt64 total_time_res;
	myInt64 reconstruct_time;
	myInt64 reconstruct_time_res = 0;
	#endif
	
	
	size_t dest_index = 0;
	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;
	

	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));

	struct modular_operation* mod_ops  = (struct modular_operation*) aligned_alloc(32,S_smpls_per_cl_block * sizeof(struct modular_operation));



	__m256i x_07,x_815,x_1623,x_2432,s_c_07,s_c_815,s_c_1623,s_c_2432,s_b_07,s_b_815,s_b_1623,s_b_2432,r_a_07,r_a_815,r_a_1623,r_a_2432,r_a_015,r_a_1632,r_a_all;
	uint32_t* ptr;

	
	__m256i shift_index_07 = _mm256_set_epi32(0,1,2,3,4,5,6,7);
	__m256i shift_index_815 = _mm256_set_epi32(8,9,10,11,12,13,14,15);
	__m256i shift_index_1623 = _mm256_set_epi32(16,17,18,19,20,21,22,23);
	__m256i shift_index_2432 = _mm256_set_epi32(24,25,26,27,28,29,30,31);
	__m256i index = _mm256_setr_epi32(0,16,32,48,64,80,96,112);
	__m256i mask = _mm256_set1_epi32(1);
	#ifdef _PROFILE
	total_time = start_tsc();
	#endif

	for(size_t i = 0; i < S_num_cl_blocks;++i){

		#ifdef _PROFILE
		reconstruct_time = start_tsc();
		#endif
		uint32_t * _1bit_index = dS + i * cl_block_size;
		uint32_t * _2bit_index = dS + i * cl_block_size +  cl_size * 8;
		uint32_t * _3bit_index = dS + i * cl_block_size + cl_size * 16;
		uint32_t * _4bit_Index = dS + i * cl_block_size + cl_size * 24;

		// size_t dest_index = 0;
		size_t s_buffer_index = 0;
		for(size_t  j = 0; j < cl_size;++j){

			x_07 = _mm256_i32gather_epi32(_1bit_index,index,4);
			x_815 = _mm256_i32gather_epi32(_2bit_index,index,4);
			x_1623 = _mm256_i32gather_epi32(_3bit_index,index,4);
			x_2432 = _mm256_i32gather_epi32(_4bit_Index,index,4);
			for(size_t k = 0; k < 32;k+=S_cols){
				// printf("%d\n",k);

				// FOR S.b ===================================
				s_b_07 = _mm256_srli_epi32(x_07, k + 1 );
				s_b_815 = _mm256_srli_epi32(x_815, k + 1);
				s_b_1623 = _mm256_srli_epi32(x_1623, k  + 1);
				s_b_2432 = _mm256_srli_epi32(x_2432, k  + 1);

				s_b_07 = _mm256_and_si256(s_b_07,mask);
				s_b_815 = _mm256_and_si256(s_b_815,mask);
				s_b_1623 = _mm256_and_si256(s_b_1623,mask);
				s_b_2432 = _mm256_and_si256(s_b_2432,mask);

				s_b_07 = _mm256_sllv_epi32(s_b_07,shift_index_2432);
				s_b_815 = _mm256_sllv_epi32(s_b_815,shift_index_1623);
				s_b_1623 = _mm256_sllv_epi32(s_b_1623,shift_index_815);
				s_b_2432 = _mm256_sllv_epi32(s_b_2432,shift_index_07);


				__m256i s_b_015 = _mm256_or_si256  (s_b_07,s_b_815);
				__m256i s_b_1632 = _mm256_or_si256  (s_b_1623,s_b_2432);
				__m256i s_b_all = _mm256_or_si256  (s_b_015,s_b_1632);

				ptr = (uint32_t*) &s_b_all;
				uint32_t s_b = 0;
				for(size_t m = 0;m < 8;++m){
					s_b += ptr[m];
				}


				// FOR S.c ===================================
				s_c_07 = _mm256_srli_epi32(x_07, k + 2 );
				s_c_815 = _mm256_srli_epi32(x_815, k + 2);
				s_c_1623 = _mm256_srli_epi32(x_1623, k  + 2);
				s_c_2432 = _mm256_srli_epi32(x_2432, k  + 2);

				s_c_07 = _mm256_and_si256(s_c_07,mask);
				s_c_815 = _mm256_and_si256(s_c_815,mask);
				s_c_1623 = _mm256_and_si256(s_c_1623,mask);
				s_c_2432 = _mm256_and_si256(s_c_2432,mask);

				s_c_07 = _mm256_sllv_epi32(s_c_07,shift_index_2432);
				s_c_815 = _mm256_sllv_epi32(s_c_815,shift_index_1623);
				s_c_1623 = _mm256_sllv_epi32(s_c_1623,shift_index_815);
				s_c_2432 = _mm256_sllv_epi32(s_c_2432,shift_index_07);


				__m256i s_c_015 = _mm256_or_si256  (s_c_07,s_c_815);
				__m256i s_c_1632 = _mm256_or_si256  (s_c_1623,s_c_2432);
				__m256i s_c_all = _mm256_or_si256  (s_c_015,s_c_1632);

				ptr = (uint32_t*) &s_c_all;
				uint32_t s_c = 0;
				for(size_t m = 0;m < 8;++m){
					s_c += ptr[m];
				}

				S_b_buffer[s_buffer_index] = s_b;
				S_c_buffer[s_buffer_index] = s_c;
				s_buffer_index++;
			}
			_1bit_index++;
			_2bit_index++;
			_3bit_index++;
			_4bit_Index++;

		}
		#ifdef _PROFILE
		myInt64 end = stop_tsc(reconstruct_time);
		reconstruct_time_res += end;
		#endif

		//Computation for fast modulo 

		for(size_t k = 0; k < S_smpls_per_cl_block;++k){ 
			uint32_t L,L2;
			uint32_t d = S_b_buffer[k];
			switch(d){
				case 0: 
					
					continue; // here we just keep going and do not compute the values
					break;
				case 1:
					mod_ops[k].m = 1; mod_ops[k].sh1 = mod_ops[k].sh2 = 0;
					break;
				case 2:  
					mod_ops[k].m = 1; mod_ops[k].sh1 = 1; mod_ops[k].sh2 = 0;
					break;
				default:
					__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );  		//ceil(log2)
					L += 1;												
					L2 = (uint32_t) (L < 32? 1 << L : 0);
					mod_ops[k].m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
					mod_ops[k].sh2 = L-1;

					mod_ops[k].sh1 = 1; 
				}

		}

		// Reconcstruct all R values:


		for(size_t p = 0; p < R_num_cl_blocks;++p){
			#ifdef _PROFILE
			reconstruct_time = start_tsc();
			#endif
			_1bit_index = dR + p * cl_block_size;
			_2bit_index = dR + p * cl_block_size+  cl_size * 8;
			_3bit_index = dR + p * cl_block_size + cl_size * 16;
			_4bit_Index = dR + p * cl_block_size + cl_size * 24;
			size_t r_a_buffer_index = 0;
			for(size_t o = 0; o < cl_size; ++o){

				x_07 = _mm256_i32gather_epi32(_1bit_index,index,4);
				x_815 = _mm256_i32gather_epi32(_2bit_index,index,4);
				x_1623 = _mm256_i32gather_epi32(_3bit_index,index,4);
				x_2432 = _mm256_i32gather_epi32(_4bit_Index,index,4);
				for(size_t k = 0; k < 32;k+=R_cols){
					r_a_07 = _mm256_srli_epi32(x_07, k );
					r_a_815 = _mm256_srli_epi32(x_815, k);
					r_a_1623 = _mm256_srli_epi32(x_1623, k );
					r_a_2432 = _mm256_srli_epi32(x_2432, k );


					r_a_07 = _mm256_and_si256(r_a_07,mask);
					r_a_815 = _mm256_and_si256(r_a_815,mask);
					r_a_1623 = _mm256_and_si256(r_a_1623,mask);
					r_a_2432 = _mm256_and_si256(r_a_2432,mask);


					r_a_07 = _mm256_sllv_epi32(r_a_07,shift_index_2432);
					r_a_815 = _mm256_sllv_epi32(r_a_815,shift_index_1623);
					r_a_1623 = _mm256_sllv_epi32(r_a_1623,shift_index_815);
					r_a_2432 = _mm256_sllv_epi32(r_a_2432,shift_index_07);

					r_a_015 = _mm256_or_si256  (r_a_07,r_a_815);
					r_a_1632 = _mm256_or_si256  (r_a_1623,r_a_2432);
					r_a_all = _mm256_or_si256  (r_a_015,r_a_1632);

					ptr = (uint32_t*) &r_a_all;
					uint32_t r_a = 0;
					for(size_t m = 0;m < 8;++m){
						r_a += ptr[m];
					}
					R_a_buffer[r_a_buffer_index] = r_a;
					r_a_buffer_index++;
				}

				_1bit_index++;
				_2bit_index++;
				_3bit_index++;
				_4bit_Index++;
			}
			#ifdef _PROFILE
			myInt64 end = stop_tsc(reconstruct_time);
			reconstruct_time_res += end;
			#endif
			// PRINT_MALLOC_H(S_b_buffer,S_smpls_per_cl_block);LINE;
			// PRINT_MALLOC_H(R_a_buffer,R_smpls_per_cl_block);LINE;
			// PRINT_MALLOC_H(S_c_buffer,S_smpls_per_cl_block);LINE;

			for(size_t l = 0; l < S_smpls_per_cl_block;++l){
				struct modular_operation mod_l = mod_ops[l];
				uint32_t s_b = S_b_buffer[l];
				uint32_t s_c = S_c_buffer[l];
				if(s_b == 0 ){continue;}
				__m256i m_v = _mm256_set1_epi32(mod_l.m);
				__m256i d_v = _mm256_set1_epi32(s_b);
				__m256i s_c_v = _mm256_set1_epi32(s_c);
				


				for(size_t k = 0; k < R_smpls_per_cl_block; k+= 8){
					__m256i x_v = _mm256_loadu_si256(R_a_buffer + k);
					__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
					t_lo = _mm256_srli_epi64(t_lo,32);
					__m256i t_hi = _mm256_srli_epi64(x_v,32);
					t_hi = _mm256_mul_epu32(t_hi,m_v);
					__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
					__m256i res = _mm256_sub_epi32(x_v,t);
					res = _mm256_srli_epi32(res,mod_l.sh1);
					res = _mm256_add_epi32(res,t);
					res = _mm256_srli_epi32(res,mod_l.sh2);
					res = _mm256_mullo_epi32(res,d_v);
					res = _mm256_sub_epi32(x_v,res);
					__m256i eq = _mm256_cmpeq_epi32(res,s_c_v);
					uint32_t* eq_vector = &eq;

					for(size_t h = 0; h < 8; ++h){
						if(eq_vector[h] == 4294967295L){
	
							dest[2 * dest_index] = p * R_smpls_per_cl_block + k + h;
							dest[2 * dest_index + 1] = i * S_smpls_per_cl_block + l;
							dest_index++;
						}
					}
				
				
				}
			}

		} //end cl_bock R






		// PRINT_MALLOC(S_c_buffer,S_smpls_per_cl_block,1);
	} //end cl_block S
	

	#ifdef _PROFILE
	total_time_res = stop_tsc(total_time);
	printf("%lu,%lu\n",total_time_res,reconstruct_time_res);
	printf("%lf\n",100 * ((double) reconstruct_time_res)/ total_time_res);
	#endif

	*dest_rows = dest_index;
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	free(mod_ops);
}



void q3_with_prune(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
	size_t dest_index = 0;
	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;
	

	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,S_smpls_per_cl_block * sizeof(uint32_t));

	struct modular_operation* mod_ops  = (struct modular_operation*) aligned_alloc(32,S_smpls_per_cl_block * sizeof(struct modular_operation));



	__m256i x_07,x_815,x_1623,x_2432,s_c_07,s_c_815,s_c_1623,s_c_2432,s_b_07,s_b_815,s_b_1623,s_b_2432,r_a_07,r_a_815,r_a_1623,r_a_2432,r_a_015,r_a_1632,r_a_all;
	uint32_t* ptr;

	
	__m256i shift_index_07 = _mm256_set_epi32(0,1,2,3,4,5,6,7);
	__m256i shift_index_815 = _mm256_set_epi32(8,9,10,11,12,13,14,15);
	__m256i shift_index_1623 = _mm256_set_epi32(16,17,18,19,20,21,22,23);
	__m256i shift_index_2432 = _mm256_set_epi32(24,25,26,27,28,29,30,31);
	__m256i index = _mm256_setr_epi32(0,16,32,48,64,80,96,112);
	__m256i mask = _mm256_set1_epi32(1);


	for(size_t i = 0; i < S_num_cl_blocks;++i){
		uint32_t * _1bit_index = dS + i * cl_block_size;
		uint32_t * _2bit_index = dS + i * cl_block_size+  cl_size * 8;
		uint32_t * _3bit_index = dS + i * cl_block_size + cl_size * 16;
		uint32_t * _4bit_Index = dS + i * cl_block_size + cl_size * 24;

		// size_t dest_index = 0;
		size_t s_buffer_index = 0;
		for(size_t  j = 0; j < cl_size;++j){

			x_07 = _mm256_i32gather_epi32(_1bit_index,index,4);
			x_815 = _mm256_i32gather_epi32(_2bit_index,index,4);
			x_1623 = _mm256_i32gather_epi32(_3bit_index,index,4);
			x_2432 = _mm256_i32gather_epi32(_4bit_Index,index,4);
			for(size_t k = 0; k < 32;k+=S_cols){
				// printf("%d\n",k);

				// FOR S.b ===================================
				s_b_07 = _mm256_srli_epi32(x_07, k + 1 );
				s_b_815 = _mm256_srli_epi32(x_815, k + 1);
				s_b_1623 = _mm256_srli_epi32(x_1623, k  + 1);
				s_b_2432 = _mm256_srli_epi32(x_2432, k  + 1);

				s_b_07 = _mm256_and_si256(s_b_07,mask);
				s_b_815 = _mm256_and_si256(s_b_815,mask);
				s_b_1623 = _mm256_and_si256(s_b_1623,mask);
				s_b_2432 = _mm256_and_si256(s_b_2432,mask);

				s_b_07 = _mm256_sllv_epi32(s_b_07,shift_index_2432);
				s_b_815 = _mm256_sllv_epi32(s_b_815,shift_index_1623);
				s_b_1623 = _mm256_sllv_epi32(s_b_1623,shift_index_815);
				s_b_2432 = _mm256_sllv_epi32(s_b_2432,shift_index_07);


				__m256i s_b_015 = _mm256_or_si256  (s_b_07,s_b_815);
				__m256i s_b_1632 = _mm256_or_si256  (s_b_1623,s_b_2432);
				__m256i s_b_all = _mm256_or_si256  (s_b_015,s_b_1632);

				ptr = (uint32_t*) &s_b_all;
				uint32_t s_b = 0;
				for(size_t m = 0;m < 8;++m){
					s_b += ptr[m];
				}


				// FOR S.c ===================================
				s_c_07 = _mm256_srli_epi32(x_07, k + 2 );
				s_c_815 = _mm256_srli_epi32(x_815, k + 2);
				s_c_1623 = _mm256_srli_epi32(x_1623, k  + 2);
				s_c_2432 = _mm256_srli_epi32(x_2432, k  + 2);

				s_c_07 = _mm256_and_si256(s_c_07,mask);
				s_c_815 = _mm256_and_si256(s_c_815,mask);
				s_c_1623 = _mm256_and_si256(s_c_1623,mask);
				s_c_2432 = _mm256_and_si256(s_c_2432,mask);

				s_c_07 = _mm256_sllv_epi32(s_c_07,shift_index_2432);
				s_c_815 = _mm256_sllv_epi32(s_c_815,shift_index_1623);
				s_c_1623 = _mm256_sllv_epi32(s_c_1623,shift_index_815);
				s_c_2432 = _mm256_sllv_epi32(s_c_2432,shift_index_07);


				__m256i s_c_015 = _mm256_or_si256  (s_c_07,s_c_815);
				__m256i s_c_1632 = _mm256_or_si256  (s_c_1623,s_c_2432);
				__m256i s_c_all = _mm256_or_si256  (s_c_015,s_c_1632);

				ptr = (uint32_t*) &s_c_all;
				uint32_t s_c = 0;
				for(size_t m = 0;m < 8;++m){
					s_c += ptr[m];
				}

				S_b_buffer[s_buffer_index] = s_b;
				S_c_buffer[s_buffer_index] = s_c;
				s_buffer_index++;
			}
			_1bit_index++;
			_2bit_index++;
			_3bit_index++;
			_4bit_Index++;

		}

		

		//Computation for fast modulo 

		for(size_t k = 0; k < S_smpls_per_cl_block;++k){ 
			uint32_t L,L2;
			uint32_t d = S_b_buffer[k];
			if(S_c_buffer[k] > d){ continue;}
			switch(d){
				case 0: 
					
					continue; // here we just keep going and do not compute the values
					break;
				case 1:
					mod_ops[k].m = 1; mod_ops[k].sh1 = mod_ops[k].sh2 = 0;
					break;
				case 2:  
					mod_ops[k].m = 1; mod_ops[k].sh1 = 1; mod_ops[k].sh2 = 0;
					break;
				default:
					__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );  		//ceil(log2)
					L += 1;												
					L2 = (uint32_t) (L < 32? 1 << L : 0);
					mod_ops[k].m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
					mod_ops[k].sh2 = L-1;
					
					mod_ops[k].sh1 = 1; 
				}

		}

		// Reconcstruct all R values:


		for(size_t p = 0; p < R_num_cl_blocks;++p){
			_1bit_index = dR + p * cl_block_size;
			_2bit_index = dR + p * cl_block_size+  cl_size * 8;
			_3bit_index = dR + p * cl_block_size + cl_size * 16;
			_4bit_Index = dR + p * cl_block_size + cl_size * 24;
			size_t r_a_buffer_index = 0;
			for(size_t o = 0; o < cl_size; ++o){

				x_07 = _mm256_i32gather_epi32(_1bit_index,index,4);
				x_815 = _mm256_i32gather_epi32(_2bit_index,index,4);
				x_1623 = _mm256_i32gather_epi32(_3bit_index,index,4);
				x_2432 = _mm256_i32gather_epi32(_4bit_Index,index,4);
				for(size_t k = 0; k < 32;k+=R_cols){
					r_a_07 = _mm256_srli_epi32(x_07, k );
					r_a_815 = _mm256_srli_epi32(x_815, k);
					r_a_1623 = _mm256_srli_epi32(x_1623, k );
					r_a_2432 = _mm256_srli_epi32(x_2432, k );


					r_a_07 = _mm256_and_si256(r_a_07,mask);
					r_a_815 = _mm256_and_si256(r_a_815,mask);
					r_a_1623 = _mm256_and_si256(r_a_1623,mask);
					r_a_2432 = _mm256_and_si256(r_a_2432,mask);


					r_a_07 = _mm256_sllv_epi32(r_a_07,shift_index_2432);
					r_a_815 = _mm256_sllv_epi32(r_a_815,shift_index_1623);
					r_a_1623 = _mm256_sllv_epi32(r_a_1623,shift_index_815);
					r_a_2432 = _mm256_sllv_epi32(r_a_2432,shift_index_07);

					r_a_015 = _mm256_or_si256  (r_a_07,r_a_815);
					r_a_1632 = _mm256_or_si256  (r_a_1623,r_a_2432);
					r_a_all = _mm256_or_si256  (r_a_015,r_a_1632);

					ptr = (uint32_t*) &r_a_all;
					uint32_t r_a = 0;
					for(size_t m = 0;m < 8;++m){
						r_a += ptr[m];
					}
					R_a_buffer[r_a_buffer_index] = r_a;
					r_a_buffer_index++;
				}

				_1bit_index++;
				_2bit_index++;
				_3bit_index++;
				_4bit_Index++;
			}
			
			// PRINT_MALLOC_H(S_b_buffer,S_smpls_per_cl_block);LINE;
			// PRINT_MALLOC_H(R_a_buffer,R_smpls_per_cl_block);LINE;
			// PRINT_MALLOC_H(S_c_buffer,S_smpls_per_cl_block);LINE;

			for(size_t l = 0; l < S_smpls_per_cl_block;++l){
				struct modular_operation mod_l = mod_ops[l];
				uint32_t s_b = S_b_buffer[l];
				uint32_t s_c = S_c_buffer[l];
				if(s_b == 0 ){continue;}
				__m256i m_v = _mm256_set1_epi32(mod_l.m);
				__m256i d_v = _mm256_set1_epi32(s_b);
				__m256i s_c_v = _mm256_set1_epi32(s_c);
				


				for(size_t k = 0; k < R_smpls_per_cl_block; k+= 8){
					__m256i x_v = _mm256_loadu_si256(R_a_buffer + k);
					__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
					t_lo = _mm256_srli_epi64(t_lo,32);
					__m256i t_hi = _mm256_srli_epi64(x_v,32);
					t_hi = _mm256_mul_epu32(t_hi,m_v);
					__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
					__m256i res = _mm256_sub_epi32(x_v,t);
					res = _mm256_srli_epi32(res,mod_l.sh1);
					res = _mm256_add_epi32(res,t);
					res = _mm256_srli_epi32(res,mod_l.sh2);
					res = _mm256_mullo_epi32(res,d_v);
					res = _mm256_sub_epi32(x_v,res);
					__m256i eq = _mm256_cmpeq_epi32(res,s_c_v);
					uint32_t* eq_vector = &eq;

					for(size_t h = 0; h < 8; ++h){
						if(eq_vector[h] == 4294967295L){
	
							dest[2 * dest_index] = p * R_smpls_per_cl_block + k + h;
							dest[2 * dest_index + 1] = i * S_smpls_per_cl_block + l;
							dest_index++;
						}
					}
				
				
				}
			}

		} //end cl_bock R






		// PRINT_MALLOC(S_c_buffer,S_smpls_per_cl_block,1);
	} //end cl_block S

	*dest_rows = dest_index;
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	free(mod_ops);
}



void q3_l2_blocking(uint32_t *dR, uint32_t *dS, uint32_t * dest,size_t * dest_rows, size_t R_rows, size_t R_cols, size_t S_rows, size_t S_cols,size_t wordsize, size_t cl_size){
	#ifdef _PROFILE
	myInt64 total_time;
	myInt64 total_time_res;
	myInt64 reconstruct_time;
	myInt64 reconstruct_time_res = 0;
	#endif
	
	// @parameterize

	size_t num_s_blocks = 32;
	
	size_t dest_index = 0;
	size_t cl_block_size = wordsize * cl_size;
	
	size_t R_num_cl_blocks = (R_rows * R_cols) / cl_block_size;  
	size_t R_smpls_per_cl_block = cl_block_size / R_cols;
	size_t R_samples_per_entry = 32 / R_cols;
	size_t R_samples_per_cl = cl_size * R_samples_per_entry;

	size_t S_num_cl_blocks = (S_rows * S_cols) / cl_block_size;
	size_t S_smpls_per_cl_block = cl_block_size / S_cols;
	size_t S_samples_per_entry = 32 / S_cols;
	size_t S_samples_per_cl = cl_size * S_samples_per_entry;
	


	
	size_t s_block_size = S_num_cl_blocks / num_s_blocks;

	uint32_t * R_a_buffer = (uint32_t *) aligned_alloc(32,R_smpls_per_cl_block * sizeof(uint32_t));
	uint32_t * S_b_buffer = (uint32_t *) aligned_alloc(32,num_s_blocks * S_smpls_per_cl_block * sizeof(uint32_t));
	
	uint32_t * S_c_buffer = (uint32_t *) aligned_alloc(32,num_s_blocks * S_smpls_per_cl_block * sizeof(uint32_t));

	struct modular_operation* mod_ops  = (struct modular_operation*) aligned_alloc(32,num_s_blocks * S_smpls_per_cl_block * sizeof(struct modular_operation));


	




	__m256i x_07,x_815,x_1623,x_2432,s_c_07,s_c_815,s_c_1623,s_c_2432,s_b_07,s_b_815,s_b_1623,s_b_2432,r_a_07,r_a_815,r_a_1623,r_a_2432,r_a_015,r_a_1632,r_a_all;
	uint32_t* ptr;

	
	__m256i shift_index_07 = _mm256_set_epi32(0,1,2,3,4,5,6,7);
	__m256i shift_index_815 = _mm256_set_epi32(8,9,10,11,12,13,14,15);
	__m256i shift_index_1623 = _mm256_set_epi32(16,17,18,19,20,21,22,23);
	__m256i shift_index_2432 = _mm256_set_epi32(24,25,26,27,28,29,30,31);
	__m256i index = _mm256_setr_epi32(0,16,32,48,64,80,96,112);
	__m256i mask = _mm256_set1_epi32(1);
	#ifdef _PROFILE
	total_time = start_tsc();
	#endif
	

	// 
	size_t s_block_index = 0;
	for(size_t i = 0; i < S_num_cl_blocks;i+= num_s_blocks){
		// size_t start_index_for_block = i;
		size_t s_buffer_index = 0;
		for(size_t s_block = 0; s_block < num_s_blocks;s_block++){
			#ifdef _PROFILE
			reconstruct_time = start_tsc();
			#endif
			//  = i * S_num_cl_blocks /

			uint32_t * _1bit_index = dS + s_block_index * cl_block_size;
			uint32_t * _2bit_index = dS + s_block_index * cl_block_size +  cl_size * 8;
			uint32_t * _3bit_index = dS + s_block_index * cl_block_size + cl_size * 16;
			uint32_t * _4bit_Index = dS + s_block_index * cl_block_size + cl_size * 24;

			// size_t dest_index = 0;
			
			for(size_t  j = 0; j < cl_size;++j){

				x_07 = _mm256_i32gather_epi32(_1bit_index,index,4);
				x_815 = _mm256_i32gather_epi32(_2bit_index,index,4);
				x_1623 = _mm256_i32gather_epi32(_3bit_index,index,4);
				x_2432 = _mm256_i32gather_epi32(_4bit_Index,index,4);
				for(size_t k = 0; k < 32;k+=S_cols){
					// printf("%d\n",k);

					// FOR S.b ===================================
					s_b_07 = _mm256_srli_epi32(x_07, k + 1 );
					s_b_815 = _mm256_srli_epi32(x_815, k + 1);
					s_b_1623 = _mm256_srli_epi32(x_1623, k  + 1);
					s_b_2432 = _mm256_srli_epi32(x_2432, k  + 1);

					s_b_07 = _mm256_and_si256(s_b_07,mask);
					s_b_815 = _mm256_and_si256(s_b_815,mask);
					s_b_1623 = _mm256_and_si256(s_b_1623,mask);
					s_b_2432 = _mm256_and_si256(s_b_2432,mask);

					s_b_07 = _mm256_sllv_epi32(s_b_07,shift_index_2432);
					s_b_815 = _mm256_sllv_epi32(s_b_815,shift_index_1623);
					s_b_1623 = _mm256_sllv_epi32(s_b_1623,shift_index_815);
					s_b_2432 = _mm256_sllv_epi32(s_b_2432,shift_index_07);


					__m256i s_b_015 = _mm256_or_si256  (s_b_07,s_b_815);
					__m256i s_b_1632 = _mm256_or_si256  (s_b_1623,s_b_2432);
					__m256i s_b_all = _mm256_or_si256  (s_b_015,s_b_1632);

					ptr = (uint32_t*) &s_b_all;
					uint32_t s_b = 0;
					for(size_t m = 0;m < 8;++m){
						s_b += ptr[m];
					}


					// FOR S.c ===================================
					s_c_07 = _mm256_srli_epi32(x_07, k + 2 );
					s_c_815 = _mm256_srli_epi32(x_815, k + 2);
					s_c_1623 = _mm256_srli_epi32(x_1623, k  + 2);
					s_c_2432 = _mm256_srli_epi32(x_2432, k  + 2);

					s_c_07 = _mm256_and_si256(s_c_07,mask);
					s_c_815 = _mm256_and_si256(s_c_815,mask);
					s_c_1623 = _mm256_and_si256(s_c_1623,mask);
					s_c_2432 = _mm256_and_si256(s_c_2432,mask);

					s_c_07 = _mm256_sllv_epi32(s_c_07,shift_index_2432);
					s_c_815 = _mm256_sllv_epi32(s_c_815,shift_index_1623);
					s_c_1623 = _mm256_sllv_epi32(s_c_1623,shift_index_815);
					s_c_2432 = _mm256_sllv_epi32(s_c_2432,shift_index_07);


					__m256i s_c_015 = _mm256_or_si256  (s_c_07,s_c_815);
					__m256i s_c_1632 = _mm256_or_si256  (s_c_1623,s_c_2432);
					__m256i s_c_all = _mm256_or_si256  (s_c_015,s_c_1632);

					ptr = (uint32_t*) &s_c_all;
					uint32_t s_c = 0;
					for(size_t m = 0;m < 8;++m){
						s_c += ptr[m];
					}

					S_b_buffer[s_buffer_index] = s_b;
					S_c_buffer[s_buffer_index] = s_c;
					s_buffer_index++;
				}
				_1bit_index++;
				_2bit_index++;
				_3bit_index++;
				_4bit_Index++;

			}
			#ifdef _PROFILE
			myInt64 end = stop_tsc(reconstruct_time);
			reconstruct_time_res += end;
			#endif




			s_block_index++;
		}

		// PRINT_MALLOC(S_b_buffer,S_smpls_per_cl_block * S_num_cl_blocks,1);



		//Computation for fast modulo 

		for(size_t k = 0; k < S_smpls_per_cl_block * num_s_blocks;++k){ 
			// printf("K: %d\n",k);
			uint32_t L,L2;
			uint32_t d = S_b_buffer[k];
			switch(d){
				case 0: 
					
					continue; // here we just keep going and do not compute the values
					break;
				case 1:
					mod_ops[k].m = 1; mod_ops[k].sh1 = mod_ops[k].sh2 = 0;
					break;
				case 2:  
					mod_ops[k].m = 1; mod_ops[k].sh1 = 1; mod_ops[k].sh2 = 0;
					break;
				default:
					__asm("bsrl %1, %0" : "=r"(L) : "r"(d-1) : );  		//ceil(log2)
					L += 1;												
					L2 = (uint32_t) (L < 32? 1 << L : 0);
					mod_ops[k].m = 1 + (((uint64_t) (L2 - d)) << 32) / d;
					mod_ops[k].sh2 = L-1;

					mod_ops[k].sh1 = 1; 
				}

		}


			// Reconcstruct all R values:


			for(size_t p = 0; p < R_num_cl_blocks;++p){
				#ifdef _PROFILE
				reconstruct_time = start_tsc();
				#endif
				uint32_t* _1bit_index = dR + p * cl_block_size;
				uint32_t* _2bit_index = dR + p * cl_block_size+  cl_size * 8;
				uint32_t* _3bit_index = dR + p * cl_block_size + cl_size * 16;
				uint32_t* _4bit_Index = dR + p * cl_block_size + cl_size * 24;
				size_t r_a_buffer_index = 0;
				for(size_t o = 0; o < cl_size; ++o){

					x_07 = _mm256_i32gather_epi32(_1bit_index,index,4);
					x_815 = _mm256_i32gather_epi32(_2bit_index,index,4);
					x_1623 = _mm256_i32gather_epi32(_3bit_index,index,4);
					x_2432 = _mm256_i32gather_epi32(_4bit_Index,index,4);
					for(size_t k = 0; k < 32;k+=R_cols){
						r_a_07 = _mm256_srli_epi32(x_07, k );
						r_a_815 = _mm256_srli_epi32(x_815, k);
						r_a_1623 = _mm256_srli_epi32(x_1623, k );
						r_a_2432 = _mm256_srli_epi32(x_2432, k );


						r_a_07 = _mm256_and_si256(r_a_07,mask);
						r_a_815 = _mm256_and_si256(r_a_815,mask);
						r_a_1623 = _mm256_and_si256(r_a_1623,mask);
						r_a_2432 = _mm256_and_si256(r_a_2432,mask);


						r_a_07 = _mm256_sllv_epi32(r_a_07,shift_index_2432);
						r_a_815 = _mm256_sllv_epi32(r_a_815,shift_index_1623);
						r_a_1623 = _mm256_sllv_epi32(r_a_1623,shift_index_815);
						r_a_2432 = _mm256_sllv_epi32(r_a_2432,shift_index_07);

						r_a_015 = _mm256_or_si256  (r_a_07,r_a_815);
						r_a_1632 = _mm256_or_si256  (r_a_1623,r_a_2432);
						r_a_all = _mm256_or_si256  (r_a_015,r_a_1632);

						ptr = (uint32_t*) &r_a_all;
						uint32_t r_a = 0;
						for(size_t m = 0;m < 8;++m){
							r_a += ptr[m];
						}
						R_a_buffer[r_a_buffer_index] = r_a;
						r_a_buffer_index++;
					}

					_1bit_index++;
					_2bit_index++;
					_3bit_index++;
					_4bit_Index++;
				}
				#ifdef _PROFILE
				myInt64 end = stop_tsc(reconstruct_time);
				reconstruct_time_res += end;
				#endif
				// PRINT_MALLOC_H(S_b_buffer,S_smpls_per_cl_block);LINE;
				// PRINT_MALLOC_H(R_a_buffer,R_smpls_per_cl_block);LINE;
				// PRINT_MALLOC_H(S_c_buffer,S_smpls_per_cl_block);LINE;

				for(size_t l = 0; l < S_smpls_per_cl_block * num_s_blocks;++l){
					struct modular_operation mod_l = mod_ops[l];
					uint32_t s_b = S_b_buffer[l];
					uint32_t s_c = S_c_buffer[l];
					if(s_b == 0 ){continue;}
					__m256i m_v = _mm256_set1_epi32(mod_l.m);
					__m256i d_v = _mm256_set1_epi32(s_b);
					__m256i s_c_v = _mm256_set1_epi32(s_c);
					


					for(size_t k = 0; k < R_smpls_per_cl_block; k+= 8){
						__m256i x_v = _mm256_loadu_si256(R_a_buffer + k);
						__m256i t_lo = _mm256_mul_epu32(x_v,m_v);
						t_lo = _mm256_srli_epi64(t_lo,32);
						__m256i t_hi = _mm256_srli_epi64(x_v,32);
						t_hi = _mm256_mul_epu32(t_hi,m_v);
						__m256i t = _mm256_blend_epi32(t_lo,t_hi,0b10101010);
						__m256i res = _mm256_sub_epi32(x_v,t);
						res = _mm256_srli_epi32(res,mod_l.sh1);
						res = _mm256_add_epi32(res,t);
						res = _mm256_srli_epi32(res,mod_l.sh2);
						res = _mm256_mullo_epi32(res,d_v);
						res = _mm256_sub_epi32(x_v,res);
						__m256i eq = _mm256_cmpeq_epi32(res,s_c_v);
						uint32_t* eq_vector = &eq;

						for(size_t h = 0; h < 8; ++h){
							if(eq_vector[h] == 4294967295L){
		
								dest[2 * dest_index] = p * R_smpls_per_cl_block + k + h;
								dest[2 * dest_index + 1] = i * S_smpls_per_cl_block + l;
								dest_index++;
							}
						}
					
					
					}
				}

		} //end cl_bock R






		// PRINT_MALLOC(S_c_buffer,S_smpls_per_cl_block,1);
	} //end cl_block S
	// }// end s_block 

	#ifdef _PROFILE
	total_time_res = stop_tsc(total_time);
	printf("%lu,%lu\n",total_time_res,reconstruct_time_res);
	printf("%lf\n",100 * ((double) reconstruct_time_res)/ total_time_res);
	#endif

	*dest_rows = dest_index;
	free(R_a_buffer);
	free(S_b_buffer);
	free(S_c_buffer);
	free(mod_ops);
}
