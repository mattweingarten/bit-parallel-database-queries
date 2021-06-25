
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/query2.h"
#include <immintrin.h>

//Select SUM(c) FROM R WHERE R.a < R.b

//Straightforward
//TODO


void print256_num(__m256i var)
{
    uint32_t val[8];
    memcpy(val, &var, sizeof(val));
    printf("Numerical: %i %i %i %i %i %i %i %i \n", 
           val[0], val[1], val[2], val[3], val[4], val[5], 
           val[6], val[7]);
}

uint64_t q2(uint32_t * data,int rows,int cols){
    uint64_t sum = 0;
    for(int i = 0;i < rows;++i){
        // printf("[%d],res=%u,val=%u\n",i,data[i * cols + 0] < data[i * cols + 1],data[i * cols + 2]);
        if(data[i * cols + 0] < data[i * cols + 1]){
            sum += data[i * cols + 2];
        }
        
    }
    
    return sum;
}

uint64_t q2_weave(uint32_t * data,uint32_t * cond_buffer,uint32_t *temp_buffer,uint32_t *sum_buffer,int word_size,int block_size, int num_samples,int num_features,int number_entries){

    // printf("Got here!\n");
    int chunk_index;
	int feature_index;
	uint32_t a;
	uint32_t b;
    uint32_t c;
	uint32_t xor;
    uint64_t sum;

    int samples_per_word = word_size / num_features;
	int samples_per_block = block_size / num_features;
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
    int cols_per_block = word_size;
    
    memset(sum_buffer,0,4 * samples_per_block);
    memset(temp_buffer,0,4 * samples_per_block);
    memset(cond_buffer,0,4 * samples_per_block);

    sum = 0;
    for(int k = 0; k < num_blocks;k++){
        for(int j = 0; j < word_size;++j){ 
            for(int i = 0; i < samples_per_block; ++i){ 
                chunk_index = i / samples_per_word; 
                feature_index = i % samples_per_word; 
                a = data[k * block_size + rows_per_block * j  + chunk_index] >> (feature_index * num_features) &1;
                b = data[k * block_size + rows_per_block * j  + chunk_index] >> (feature_index * num_features + 1) &1;
                c = data[k * block_size + rows_per_block * j  + chunk_index] >> (feature_index * num_features + 2) &1;
                xor =  a ^ b;
                cond_buffer[i] = cond_buffer[i] | ((xor & b) & (!temp_buffer[i]));
                temp_buffer[i] = temp_buffer[i] | (xor & a);
                sum_buffer[i] =  sum_buffer[i] + (c << (31 - j));
                // printf("%d\n",(31 - j));   
            }

	    }

        // TODO: this part should be easily vectorizable and we can multiple accumalators!
        for(int i = 0; i < samples_per_block;++i){
            // printf("[%d]:, res_buffer=[%u],sumbuffer=%u,\n",i,cond_buffer[i],sum_buffer[i]);



            if(cond_buffer[i]){ // use mm_256_blendvd for branch elimination
                sum += sum_buffer[i]; // use acc for sum
            }
        }
        memset(sum_buffer,0,4 * samples_per_block);
        memset(temp_buffer,0,4 * samples_per_block);
        memset(cond_buffer,0,4 * samples_per_block);
        // PRINT_MALLOC(sum_buffer,samples_per_block,1); 
        //
        
    }
    return sum;
}


uint64_t q2_weave_v2(uint32_t * data,uint32_t * cond_buffer,uint32_t *temp_buffer,uint32_t *sum_buffer,int word_size,int block_size, int num_samples,int num_features,int number_entries){
//void* q2_weave_v2(uint32_t * data,uint32_t * cond_buffer,uint32_t *temp_buffer,uint32_t *sum_buffer,int word_size,int block_size, int num_samples,int num_features,int number_entries){
    int chunk_index;
    int feature_index;
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t xor;
    uint64_t sum;

    int samples_per_word = word_size / num_features;
    int samples_per_block = block_size / num_features;
    // block_size == 512
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
    // rows_per_block == 16
    int cols_per_block = word_size;
    // word size = 32

    sum = 0;
    for(int k = 0; k < num_blocks;k++){
        for(int j = 0; j < cols_per_block;++j){ 
            for(int m = 0; m < rows_per_block; m++){
                uint32_t load = data[k * block_size + rows_per_block * j  + m];
                for(int i = 0; i < samples_per_word; i++){
                    a = (load >> (i * num_features)) & 1;
                    b = (load >> (i * num_features + 1)) & 1;
                    c = (load >> (i * num_features + 2)) & 1;
                    xor = a ^ b;           

                    int bufferid = m*samples_per_word+i;                
                    cond_buffer[bufferid] = cond_buffer[bufferid] | ((xor & b) & (!temp_buffer[bufferid]));
                    temp_buffer[bufferid] = temp_buffer[bufferid] | (xor & a);
                    sum_buffer[bufferid] += (c << (31 - j));

                }
            } 
        }


        // TODO: this part should be easily vectorizable and we can multiple accumalators!
        for(int i = 0; i < samples_per_block;++i){
            // printf("[%d]:, res_buffer=[%u],sumbuffer=%u,\n",i,cond_buffer[i],sum_buffer[i]);

            if(cond_buffer[i]){ // use mm_256_blendvd for branch elimination
                sum += sum_buffer[i]; // use acc for sum
            }
        }

        memset(sum_buffer,0,4 * samples_per_block);
        memset(temp_buffer,0,4 * samples_per_block);
        memset(cond_buffer,0,4 * samples_per_block);
    }

    return sum;
}

uint64_t q2_weave_scalar(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size, int num_samples,int num_features,int number_entries){

    int chunk_index;
    int feature_index;
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t xor;
    uint64_t sum;

    int samples_per_word = word_size / num_features;
    int samples_per_block = block_size / num_features;
    // block_size == 512
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
    // rows_per_block == 16
    int cols_per_block = word_size;
    // word size = 32
    sum =0;
    //printf("rows%d samples%d", rows_per_block, samples_per_block);
    size_t res_idx = 0; // counts for k loop in samples
    size_t data_index_s = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
        size_t j_idx = 0; // rows per block
        for(int j = 0; j < word_size;++j){
            size_t cres_idx = res_idx; // k * samples_per_block
            for(int m = 0; m < rows_per_block; m++){
                uint32_t load = data[data_index_s + j_idx + m];
                int bit_shift = 0;
                for(int i = 0; i < samples_per_word; i++){
                    
                    size_t ci = cres_idx + i;
                    uint32_t ctemp = temps[ci];
                    
                    a = (load >> (bit_shift)) & 1;
                    b = (load >> (bit_shift + 1)) & 1;
                    c = (load >> (bit_shift + 2)) & 1;
                    xor = a ^ b;
                    uint32_t xora = xor & a;
                    uint32_t xorb = xor & b;
                    uint32_t andnotb = xorb & (!ctemp); 
                    results[ci] |= andnotb;
                    temps[ci] = ctemp | xora;
                    sums[ci] |=  (c << (31 - j));
                    //sum += (c << (31 - j));
                    bit_shift += num_features; // i * num_features
                }
                cres_idx += samples_per_word; // + samples_per_word * m
            }
            j_idx += rows_per_block;    
        }


        data_index_s += block_size;
        res_idx += samples_per_block;
    }
        // TODO: this part should be easily vectorizable and we can multiple accumalators!
        for(int i = 0; i <samples_per_block * num_blocks     ;++i){
             //printf("[%d]:, res_buffer=[%u],sumbuffer=%u,\n",i,cond_buffer[i],sum_buffer[i]);

            if(results[i]){ // use mm_256_blendvd for branch elimination
                sum += sums[i]; // use acc for sum
            }
        }
    
    return sum;
}

uint64_t q2_weave_scalar_vector_accum(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size, int num_samples,int num_features,int number_entries){

    int chunk_index;
    int feature_index;
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t xor;
    uint64_t sum;
    __m256i zeros= _mm256_setzero_si256();
    __m256i accum= _mm256_setzero_si256();
    __m256 mask;
    __m256i sumdata=_mm256_setzero_si256();

    int samples_per_word = word_size / num_features;
    int samples_per_block = block_size / num_features;
    // block_size == 512
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
    // rows_per_block == 16
    int cols_per_block = word_size;
    // word size = 32
    sum =0;
    //printf("rows%d samples%d", rows_per_block, samples_per_block);
    size_t res_idx = 0; // counts for k loop in samples
    size_t data_index_s = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
        size_t j_idx = 0; // rows per block
        for(int j = 0; j < word_size;++j){
            size_t cres_idx = res_idx; // k * samples_per_block
            for(int m = 0; m < rows_per_block; m++){
                uint32_t load = data[data_index_s + j_idx + m];
                int bit_shift = 0;
                for(int i = 0; i < samples_per_word; i++){
                    
                    size_t ci = cres_idx + i;
                    uint32_t ctemp = temps[ci];
                    
                    a = (load >> (bit_shift)) & 1;
                    b = (load >> (bit_shift + 1)) & 1;
                    c = (load >> (bit_shift + 2)) & 1;
                    xor = a ^ b;
                    uint32_t xora = xor & a;
                    uint32_t xorb = xor & b;
                    uint32_t andnotb = xorb & (!ctemp); 
                    results[ci] |= andnotb;
                    temps[ci] = ctemp | xora;
                    sums[ci] |=  (c << (31 - j));
                    //sum += (c << (31 - j));
                    bit_shift += num_features; // i * num_features
                }
                cres_idx += samples_per_word; // + samples_per_word * m
            }
            j_idx += rows_per_block;    
        }


        data_index_s += block_size;
        res_idx += samples_per_block;
    }


        //vectorized accumulation
        for(int i = 0; i <samples_per_block * num_blocks; i+=8){
            sumdata= (__m256i)_mm256_load_ps(sums+i);
            mask= _mm256_load_ps(results+i);
            mask =(__m256) _mm256_slli_epi32((__m256i)mask, 31);
            sumdata=(__m256i) _mm256_blendv_ps((__m256)zeros,(__m256)sumdata, mask);
            accum =  _mm256_add_epi32(accum, sumdata);
        }

        uint32_t accums[8];
        //memcpy(cres, &res1, sizeof(cres));
        _mm256_storeu_pd(accums,(__m256d) accum);
        sum += accums[0] + accums[1] + accums[2] + accums[3] + accums[4] + accums[5] + accums[6] + accums[7]; // need storeu so it doesn't crash (investigate?)
    

    return sum;
}


uint64_t q2_weave_unroll(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size, int num_samples,int num_features,int number_entries){
//based on q1_weave_v5(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries){
    /*
    if(num_features % 4 != 0 || num_features > 32){
        printf("can't handle num_features: %i! \n", num_features);
        return;
    }
    */
    int chunk_index;
    int feature_index;

    int samples_per_word = word_size / num_features;
    int samples_per_block = block_size / num_features;
    // block_size == 512
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
    // rows_per_block == 16
    int cols_per_block = word_size;
    // word size = 32
    uint64_t    sum = 0;
    memset(sums,0,4 * samples_per_block);
    memset(temps,0,4 * samples_per_block);
    memset(results,0,4 * samples_per_block);
    // case where we have 4 or more samples per word and unroll for the word
    if(samples_per_word >= 4){
    size_t res_idx = 0; // counts for k loop in samples
    size_t data_index_s = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
        size_t j_idx = 0; // rows per block
        for(int j = 0; j < word_size;++j){
            size_t cres_idx = res_idx; // k * samples_per_block
            for(int m = 0; m < rows_per_block; m++){


                uint32_t load = data[data_index_s + j_idx + m];


                int bit_shift = 0;
                for(int i = 0; i < samples_per_word; i += 4){
                    

                    size_t c0 = cres_idx + i;
                    size_t c2 = c0 + 2;
                    uint32_t ctemp0 = temps[c0];
                    uint32_t ctemp1 = temps[c0 + 1];
                    uint32_t ctemp2 = temps[c2];
                    uint32_t ctemp3 = temps[c2 + 1];
                                        



                    uint32_t a0 = (load >> (bit_shift)) & 1;
                    uint32_t b0 = (load >> (bit_shift + 1)) & 1;
                    uint32_t d0 = (load >> (bit_shift + 2)) & 1;
                    uint32_t xor0 = a0 ^ b0;
                    uint32_t xora0 = xor0 & a0;
                    uint32_t xorb0 = xor0 & b0;
                    uint32_t andnotb0 = xorb0 & (!ctemp0); 
                    results[c0] |= andnotb0;
                    temps[c0] = ctemp0 | xora0;
                    sums[c0] +=  (d0 << (31 - j));


                    uint32_t a1 = (load >> (bit_shift + num_features)) & 1;
                    uint32_t b1 = (load >> (bit_shift + num_features + 1)) & 1;
                    uint32_t d1 = (load >> (bit_shift + num_features + 2)) & 1;
                    uint32_t xor1 = a1 ^ b1;
                    uint32_t xora1 = xor1 & a1;
                    uint32_t xorb1 = xor1 & b1;
                    uint32_t andnotb1 = xorb1 & (!ctemp1); 
                    results[c0 + 1] |= andnotb1;
                    temps[c0 + 1] = ctemp1 | xora1;
                    sums[c0+1] +=  (d1 << (31 - j));

                    //todid

                    int bit_shift2 = bit_shift + 2 * num_features;
                    uint32_t a2 = (load >> (bit_shift2)) & 1;
                    uint32_t b2 = (load >> (bit_shift2 + 1)) & 1;
                    uint32_t d2 = (load >> (bit_shift2 + 2)) & 1;
                    uint32_t xor2 = a2 ^ b2;
                    uint32_t xora2 = xor2 & a2;
                    uint32_t xorb2 = xor2 & b2;
                    uint32_t andnotb2 = xorb2 & (!ctemp2); 
                    results[c2] |= andnotb2;
                    temps[c2] = ctemp2 | xora2;
                    sums[c2] +=  (d2 << (31 - j));
                    //todid
                    uint32_t a3 = (load >> (bit_shift2 + num_features)) & 1;
                    uint32_t b3 = (load >> (bit_shift2 + num_features + 1)) & 1;
                    uint32_t d3 = (load >> (bit_shift2 + num_features + 2)) & 1;
                    uint32_t xor3 = a3 ^ b3;
                    uint32_t xora3 = xor3 & a3;
                    uint32_t xorb3 = xor3 & b3;
                    uint32_t andnotb3 = xorb3 & (!ctemp3); 
                    results[c2 + 1] |= andnotb3;
                    temps[c2 + 1] = ctemp3 | xora3;
                    sums[c2 +1] +=  (d3 << (31 - j));

                    
                    bit_shift += 4 * num_features; // i * num_features
                }
                cres_idx += samples_per_word; // + samples_per_word * m

            }
            j_idx += rows_per_block;   

 
        }

        data_index_s += block_size;
        res_idx += samples_per_block;
    }
    // case of 16 features, unroll two words at once ?
    } else if(samples_per_word == 2){

    size_t res_idx = 0; // counts for k loop in samples
    size_t data_index = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
        for(int j = 0; j < word_size;++j){
            size_t cres_idx = res_idx; // k * samples_per_block
            for(int m = 0; m < rows_per_block; m += 2){
                uint32_t load = data[data_index];
                uint32_t load2 = data[data_index + 1];
                
                size_t c0 = cres_idx;
                size_t c2 = c0 + 2;
                uint32_t ctemp0 = temps[c0];
                uint32_t ctemp1 = temps[c0 + 1];
                uint32_t ctemp2 = temps[c2];
                uint32_t ctemp3 = temps[c2 + 1];

                uint32_t a0 = load & 1;
                uint32_t b0 = (load >> 1) & 1;
                uint32_t d0 = (load >>  + 2) & 1;

                uint32_t xor0 = a0 ^ b0;
                uint32_t xora0 = xor0 & a0;
                uint32_t xorb0 = xor0 & b0;
                uint32_t andnotb0 = xorb0 & (!ctemp0); 
                results[c0] |= andnotb0;
                temps[c0] = ctemp0 | xora0;
                sums[c0] += (d0 << (31 - j));
                //todid
                
                uint32_t a1 = (load >> num_features) & 1;
                uint32_t b1 = (load >> (num_features + 1)) & 1;
                uint32_t d1 = (load >> (num_features  + 2)) & 1;
                uint32_t xor1 = a1 ^ b1;
                uint32_t xora1 = xor1 & a1;
                uint32_t xorb1 = xor1 & b1;
                uint32_t andnotb1 = xorb1 & (!ctemp1); 
                results[c0 + 1] |= andnotb1;
                temps[c0 + 1] = ctemp1 | xora1;
                sums[c0+1] +=  (d1 << (31 - j));

                //todid

                uint32_t a2 = load2 & 1;
                uint32_t b2 = (load2 >> 1) & 1;
                uint32_t d2 = (load2 >> 2) & 1;
                uint32_t xor2 = a2 ^ b2;
                uint32_t xora2 = xor2 & a2;
                uint32_t xorb2 = xor2 & b2;
                uint32_t andnotb2 = xorb2 & (!ctemp2); 
                results[c2] |= andnotb2;
                temps[c2] = ctemp2 | xora2;
                sums[c2] += (d2 << (31 - j));

                //todid
                
                uint32_t a3 = (load2 >> (num_features)) & 1;
                uint32_t b3 = (load2 >> (num_features + 1)) & 1;
                uint32_t d3 = (load2 >> (num_features + 2)) & 1;
                uint32_t xor3 = a3 ^ b3;
                uint32_t xora3 = xor3 & a3;
                uint32_t xorb3 = xor3 & b3;
                uint32_t andnotb3 = xorb3 & (!ctemp3); 
                results[c2 + 1] |= andnotb3;
                temps[c2 + 1] = ctemp3 | xora3;
                sums[c2+1] += (d3 << (31 - j));
                //todid
                
                cres_idx += 4; // + 2 words worth of 2 samples per word
                data_index += 2;
            }
        }
        //todid

        res_idx += samples_per_block;
    }
        
        
    // goal here: UNROLL for every word of the cacheline so we don't keep load ing results and temps
    } else if(samples_per_word == 1){
        //TODO

    size_t res_idx = 0; // counts for k loop in samples
    size_t data_index = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
        // load all res / temps -> don't need to load initially!
        uint32_t res0 = 0;
        uint32_t res1 = 0;
        uint32_t res2 = 0;
        uint32_t res3 = 0;
        uint32_t res4 = 0;
        uint32_t res5 = 0;
        uint32_t res6 = 0;
        uint32_t res7 = 0;
        
        uint32_t res8 = 0;
        uint32_t res9 = 0;
        uint32_t res10 = 0;
        uint32_t res11 = 0;
        uint32_t res12 = 0;
        uint32_t res13 = 0;
        uint32_t res14 = 0;
        uint32_t res15 = 0;
        
        uint32_t temp0 = 0;
        uint32_t temp1 = 0;
        uint32_t temp2 = 0;
        uint32_t temp3 = 0;
        uint32_t temp4 = 0;
        uint32_t temp5 = 0;
        uint32_t temp6 = 0;
        uint32_t temp7 = 0;
        
        uint32_t temp8 = 0;
        uint32_t temp9 = 0;
        uint32_t temp10 = 0;
        uint32_t temp11 = 0;
        uint32_t temp12 = 0;
        uint32_t temp13 = 0;
        uint32_t temp14 = 0;
        uint32_t temp15 = 0;
        
        uint32_t sum0 = 0;
        uint32_t sum1 = 0;
        uint32_t sum2 = 0;
        uint32_t sum3 = 0;
        uint32_t sum4 = 0;
        uint32_t sum5 = 0;
        uint32_t sum6 = 0;
        uint32_t sum7 = 0;
        
        uint32_t sum8 = 0;
        uint32_t sum9 = 0;
        uint32_t sum10 = 0;
        uint32_t sum11 = 0;
        uint32_t sum12 = 0;
        uint32_t sum13 = 0;
        uint32_t sum14 = 0;
        uint32_t sum15 = 0;



        for(int j = 0; j < word_size;++j){
            uint32_t load0 = data[data_index];
            uint32_t load1 = data[data_index + 1];
            uint32_t load2 = data[data_index + 2];
            uint32_t load3 = data[data_index + 3];
            uint32_t load4 = data[data_index + 4];
            uint32_t load5 = data[data_index + 5];
            uint32_t load6 = data[data_index + 6];
            uint32_t load7 = data[data_index + 7];
            
            uint32_t load8 = data[data_index + 8];
            uint32_t load9 = data[data_index + 9];
            uint32_t load10 = data[data_index + 10];
            uint32_t load11 = data[data_index + 11];
            uint32_t load12 = data[data_index + 12];
            uint32_t load13 = data[data_index + 13];
            uint32_t load14 = data[data_index + 14];
            uint32_t load15 = data[data_index + 15];
            
            uint32_t a0 = load0 & 1;
            uint32_t b0 = (load0 >> 1) & 1;
            uint32_t d0 = (load0 >> 2) & 1;
            uint32_t xor0 = a0 ^ b0;
            uint32_t xora0 = xor0 & a0;
            uint32_t xorb0 = xor0 & b0;
            uint32_t andnotb0 = xorb0 & (!temp0); 
            res0 |= andnotb0;
            temp0 = temp0 | xora0;
            sum0 += (d0 << (31 - j));

            
            uint32_t a1 = load1 & 1;
            uint32_t b1 = (load1 >> 1) & 1;
            uint32_t d1 = (load1 >> 2) & 1;            
            uint32_t xor1 = a1 ^ b1;
            uint32_t xora1 = xor1 & a1;
            uint32_t xorb1 = xor1 & b1;
            uint32_t andnotb1 = xorb1 & (!temp1); 
            res1 |= andnotb1;
            sum1 += (d1 << (31 - j));
            
            uint32_t a2 = load2 & 1;
            uint32_t b2 = (load2 >> 1) & 1;
            uint32_t d2 = (load2 >> 2) & 1;
            uint32_t xor2 = a2 ^ b2;
            uint32_t xora2 = xor2 & a2;
            uint32_t xorb2 = xor2 & b2;
            uint32_t andnotb2 = xorb2 & (!temp2); 
            res2 |= andnotb2;
            temp2 = temp2 | xora2;
            sum2 += (d2 << (31 - j));
            

            uint32_t a3 = load3 & 1;
            uint32_t b3 = (load3 >> 1) & 1;
            uint32_t d3 = (load3 >> 2) & 1;            
            uint32_t xor3 = a3 ^ b3;
            uint32_t xora3 = xor3 & a3;
            uint32_t xorb3 = xor3 & b3;
            uint32_t andnotb3 = xorb3 & (!temp3); 
            res3 |= andnotb3;
            temp3 = temp3 | xora3;
            sum3 += (d3 << (31 - j));
            
            uint32_t a4 = load4 & 1;
            uint32_t b4 = (load4 >> 1) & 1;
            uint32_t d4 = (load4 >> 2) & 1;            
            uint32_t xor4 = a4 ^ b4;
            uint32_t xora4 = xor4 & a4;
            uint32_t xorb4 = xor4 & b4;
            uint32_t andnotb4 = xorb4 & (!temp4); 
            res4 |= andnotb4;
            temp4 = temp4 | xora4;
            sum4 += (d4 << (31 - j));

            
            uint32_t a5 = load5 & 1;
            uint32_t b5 = (load5 >> 1) & 1;
            uint32_t d5 = (load5 >> 2) & 1;            
            uint32_t xor5 = a5 ^ b5;
            uint32_t xora5 = xor5 & a5;
            uint32_t xorb5 = xor5 & b5;
            uint32_t andnotb5 = xorb5 & (!temp5); 
            res5 |= andnotb5;
            temp5 = temp5 | xora5;
            sum5 += (d5 << (31 - j));
            
            uint32_t a6 = load6 & 1;
            uint32_t b6 = (load6 >> 1) & 1;
            uint32_t d6 = (load6 >> 2) & 1;
            uint32_t xor6 = a6 ^ b6;
            uint32_t xora6 = xor6 & a6;
            uint32_t xorb6 = xor6 & b6;
            uint32_t andnotb6 = xorb6 & (!temp6); 
            res6 |= andnotb6;
            temp6 = temp6 | xora6;
            sum6 += (d6 << (31 - j));
            
            uint32_t a7 = load7 & 1;
            uint32_t b7 = (load7 >> 1) & 1;
            uint32_t d7 = (load7 >> 2) & 1;
            uint32_t xor7 = a7 ^ b7;
            uint32_t xora7 = xor7 & a7;
            uint32_t xorb7 = xor7 & b7;
            uint32_t andnotb7 = xorb7 & (!temp7); 
            res7 |= andnotb7;
            temp7 = temp7 | xora7;
            sum7 += (d7 << (31 - j));
            
            
            uint32_t a8 = load8 & 1;
            uint32_t b8 = (load8 >> 1) & 1;
            uint32_t d8 = (load8 >> 2) & 1;
            uint32_t xor8 = a8 ^ b8;
            uint32_t xora8 = xor8 & a8;
            uint32_t xorb8 = xor8 & b8;
            uint32_t andnotb8 = xorb8 & (!temp8); 
            res8 |= andnotb8;
            temp8 = temp8 | xora8;
            sum8 += (d8 << (31 - j));

            
            uint32_t a9 = load9 & 1;
            uint32_t b9 = (load9 >> 1) & 1;
            uint32_t d9 = (load9 >> 2) & 1;
            uint32_t xor9 = a9 ^ b9;
            uint32_t xora9 = xor9 & a9;
            uint32_t xorb9 = xor9 & b9;
            uint32_t andnotb9 = xorb9 & (!temp9); 
            res9 |= andnotb9;
            temp9 = temp9 | xora9;
            sum9 += (d9 << (31 - j));

            
            uint32_t a10 = load10 & 1;
            uint32_t b10 = (load10 >> 1) & 1;
            uint32_t d10 = (load10 >> 2) & 1;
            uint32_t xor10 = a10 ^ b10;
            uint32_t xora10 = xor10 & a10;
            uint32_t xorb10 = xor10 & b10;
            uint32_t andnotb10 = xorb10 & (!temp10); 
            res10 |= andnotb10;
            temp10 = temp10 | xora10;
            sum10 += (d10 << (31 - j));
            
            uint32_t a11 = load11 & 1;
            uint32_t b11 = (load11 >> 1) & 1;
            uint32_t d11 = (load11 >> 2) & 1;
            uint32_t xor11 = a11 ^ b11;
            uint32_t xora11 = xor11 & a11;
            uint32_t xorb11 = xor11 & b11;
            uint32_t andnotb11 = xorb11 & (!temp11); 
            res11 |= andnotb11;
            temp11 = temp11 | xora11;
            sum11 += (d11 << (31 - j));
            
            uint32_t a12 = load12 & 1;
            uint32_t b12 = (load12 >> 1) & 1;
            uint32_t d12 = (load12 >> 2) & 1;
            uint32_t xor12 = a12 ^ b12;
            uint32_t xora12 = xor12 & a12;
            uint32_t xorb12 = xor12 & b12;
            uint32_t andnotb12 = xorb12 & (!temp12); 
            res12 |= andnotb12;
            temp12 = temp12 | xora12;
            sum12 += (d12 << (31 - j));
            
            uint32_t a13 = load13 & 1;
            uint32_t b13 = (load13 >> 1) & 1;
            uint32_t d13 = (load13 >> 2) & 1;
            uint32_t xor13 = a13 ^ b13;
            uint32_t xora13 = xor13 & a13;
            uint32_t xorb13 = xor13 & b13;
            uint32_t andnotb13 = xorb13 & (!temp13); 
            res13 |= andnotb13;
            temp13 = temp13 | xora13;
            sum13 += (d13 << (31 - j));
            
            uint32_t a14 = load14 & 1;
            uint32_t b14 = (load14 >> 1) & 1;
            uint32_t d14 = (load14 >> 2) & 1;            
            uint32_t xor14 = a14 ^ b14;
            uint32_t xora14 = xor14 & a14;
            uint32_t xorb14 = xor14 & b14;
            uint32_t andnotb14 = xorb14 & (!temp14); 
            res14 |= andnotb14;
            temp14 = temp14 | xora14;
            sum14 += (d14 << (31 - j));
            
            uint32_t a15 = load15 & 1;
            uint32_t b15 = (load15 >> 1) & 1;
            uint32_t d15 = (load15 >> 2) & 1;
            uint32_t xor15 = a15 ^ b15;
            uint32_t xora15 = xor15 & a15;
            uint32_t xorb15 = xor15 & b15;
            uint32_t andnotb15 = xorb15 & (!temp15); 
            res15 |= andnotb15;
            temp15 = temp15 | xora15;
            sum15 += (d15 << (31 - j));
            
            data_index += 16; //words per cacheline 
        }
        
        results[res_idx] = res0;
        results[res_idx + 1] = res1;
        results[res_idx + 2] = res2;
        results[res_idx + 3] = res3;
        results[res_idx + 4] = res4;
        results[res_idx + 5] = res5;
        results[res_idx + 6] = res6;
        results[res_idx + 7] = res7;

        results[res_idx + 8] = res8;
        results[res_idx + 9] = res9;
        results[res_idx + 10] = res10;
        results[res_idx + 11] = res11;
        results[res_idx + 12] = res12;
        results[res_idx + 13] = res13;
        results[res_idx + 14] = res14;
        results[res_idx + 15] = res15;
        
        sums[res_idx] += sum0;
        sums[res_idx + 1] += sum1;
        sums[res_idx + 2] += sum2;
        sums[res_idx + 3] += sum3;
        sums[res_idx + 4] += sum4;
        sums[res_idx + 5] += sum5;
        sums[res_idx + 6] += sum6;
        sums[res_idx + 7] += sum7;

        sums[res_idx + 8] += sum8;
        sums[res_idx + 9] += sum9;
        sums[res_idx + 10] += sum10;
        sums[res_idx + 11] += sum11;
        sums[res_idx + 12] += sum12;
        sums[res_idx + 13] += sum13;
        sums[res_idx + 14] += sum14;
        sums[res_idx + 15] += sum15;


        res_idx += 16; // samples per block

    }

    }


    // TODO: this part should be easily vectorizable and we can multiple accumalators!
        for(int i = 0; i <samples_per_block * num_blocks     ;++i){
             //printf("[%d]:, res_buffer=[%u],sumbuffer=%u,\n",i,cond_buffer[i],sum_buffer[i]);

            if(results[i]){ // use mm_256_blendvd for branch elimination
                sum += sums[i]; // use acc for sum
            }
        }
    
    return sum;

}


uint64_t q2_weave_unroll_vector_accum(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size, int num_samples,int num_features,int number_entries){
//based on q1_weave_v5(uint32_t * data,uint32_t * results,uint32_t *temps,int word_size,int block_size,int num_samples,int num_features ,int number_entries){
    /*
    if(num_features % 4 != 0 || num_features > 32){
        printf("can't handle num_features: %i! \n", num_features);
        return;
    }
    */
    int chunk_index;
    int feature_index;

    int samples_per_word = word_size / num_features;
    int samples_per_block = block_size / num_features;
    // block_size == 512
    int num_blocks = ceil(number_entries / block_size);
    int rows_per_block = block_size / word_size;
    // rows_per_block == 16
    int cols_per_block = word_size;

    __m256i zeros= _mm256_setzero_si256();
    __m256i accum= _mm256_setzero_si256();
    __m256 mask;
    __m256i sumdata=_mm256_setzero_si256();
    // word size = 32
    uint64_t    sum = 0;
    memset(sums,0,4 * samples_per_block);
    memset(temps,0,4 * samples_per_block);
    memset(results,0,4 * samples_per_block);
    // case where we have 4 or more samples per word and unroll for the word
    if(samples_per_word >= 4){
    size_t res_idx = 0; // counts for k loop in samples
    size_t data_index_s = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
        size_t j_idx = 0; // rows per block
        for(int j = 0; j < word_size;++j){
            size_t cres_idx = res_idx; // k * samples_per_block
            for(int m = 0; m < rows_per_block; m++){


                uint32_t load = data[data_index_s + j_idx + m];


                int bit_shift = 0;
                for(int i = 0; i < samples_per_word; i += 4){
                    

                    size_t c0 = cres_idx + i;
                    size_t c2 = c0 + 2;
                    uint32_t ctemp0 = temps[c0];
                    uint32_t ctemp1 = temps[c0 + 1];
                    uint32_t ctemp2 = temps[c2];
                    uint32_t ctemp3 = temps[c2 + 1];
                                        



                    uint32_t a0 = (load >> (bit_shift)) & 1;
                    uint32_t b0 = (load >> (bit_shift + 1)) & 1;
                    uint32_t d0 = (load >> (bit_shift + 2)) & 1;
                    uint32_t xor0 = a0 ^ b0;
                    uint32_t xora0 = xor0 & a0;
                    uint32_t xorb0 = xor0 & b0;
                    uint32_t andnotb0 = xorb0 & (!ctemp0); 
                    results[c0] |= andnotb0;
                    temps[c0] = ctemp0 | xora0;
                    sums[c0] +=  (d0 << (31 - j));


                    uint32_t a1 = (load >> (bit_shift + num_features)) & 1;
                    uint32_t b1 = (load >> (bit_shift + num_features + 1)) & 1;
                    uint32_t d1 = (load >> (bit_shift + num_features + 2)) & 1;
                    uint32_t xor1 = a1 ^ b1;
                    uint32_t xora1 = xor1 & a1;
                    uint32_t xorb1 = xor1 & b1;
                    uint32_t andnotb1 = xorb1 & (!ctemp1); 
                    results[c0 + 1] |= andnotb1;
                    temps[c0 + 1] = ctemp1 | xora1;
                    sums[c0+1] +=  (d1 << (31 - j));

                    //todid

                    int bit_shift2 = bit_shift + 2 * num_features;
                    uint32_t a2 = (load >> (bit_shift2)) & 1;
                    uint32_t b2 = (load >> (bit_shift2 + 1)) & 1;
                    uint32_t d2 = (load >> (bit_shift2 + 2)) & 1;
                    uint32_t xor2 = a2 ^ b2;
                    uint32_t xora2 = xor2 & a2;
                    uint32_t xorb2 = xor2 & b2;
                    uint32_t andnotb2 = xorb2 & (!ctemp2); 
                    results[c2] |= andnotb2;
                    temps[c2] = ctemp2 | xora2;
                    sums[c2] +=  (d2 << (31 - j));
                    //todid
                    uint32_t a3 = (load >> (bit_shift2 + num_features)) & 1;
                    uint32_t b3 = (load >> (bit_shift2 + num_features + 1)) & 1;
                    uint32_t d3 = (load >> (bit_shift2 + num_features + 2)) & 1;
                    uint32_t xor3 = a3 ^ b3;
                    uint32_t xora3 = xor3 & a3;
                    uint32_t xorb3 = xor3 & b3;
                    uint32_t andnotb3 = xorb3 & (!ctemp3); 
                    results[c2 + 1] |= andnotb3;
                    temps[c2 + 1] = ctemp3 | xora3;
                    sums[c2 +1] +=  (d3 << (31 - j));

                    
                    bit_shift += 4 * num_features; // i * num_features
                }
                cres_idx += samples_per_word; // + samples_per_word * m

            }
            j_idx += rows_per_block;   

 
        }

        data_index_s += block_size;
        res_idx += samples_per_block;
    }
    // case of 16 features, unroll two words at once ?
    } else if(samples_per_word == 2){

    size_t res_idx = 0; // counts for k loop in samples
    size_t data_index = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
        for(int j = 0; j < word_size;++j){
            size_t cres_idx = res_idx; // k * samples_per_block
            for(int m = 0; m < rows_per_block; m += 2){
                uint32_t load = data[data_index];
                uint32_t load2 = data[data_index + 1];
                
                size_t c0 = cres_idx;
                size_t c2 = c0 + 2;
                uint32_t ctemp0 = temps[c0];
                uint32_t ctemp1 = temps[c0 + 1];
                uint32_t ctemp2 = temps[c2];
                uint32_t ctemp3 = temps[c2 + 1];

                uint32_t a0 = load & 1;
                uint32_t b0 = (load >> 1) & 1;
                uint32_t d0 = (load >>  + 2) & 1;

                uint32_t xor0 = a0 ^ b0;
                uint32_t xora0 = xor0 & a0;
                uint32_t xorb0 = xor0 & b0;
                uint32_t andnotb0 = xorb0 & (!ctemp0); 
                results[c0] |= andnotb0;
                temps[c0] = ctemp0 | xora0;
                sums[c0] += (d0 << (31 - j));
                //todid
                
                uint32_t a1 = (load >> num_features) & 1;
                uint32_t b1 = (load >> (num_features + 1)) & 1;
                uint32_t d1 = (load >> (num_features  + 2)) & 1;
                uint32_t xor1 = a1 ^ b1;
                uint32_t xora1 = xor1 & a1;
                uint32_t xorb1 = xor1 & b1;
                uint32_t andnotb1 = xorb1 & (!ctemp1); 
                results[c0 + 1] |= andnotb1;
                temps[c0 + 1] = ctemp1 | xora1;
                sums[c0+1] +=  (d1 << (31 - j));

                //todid

                uint32_t a2 = load2 & 1;
                uint32_t b2 = (load2 >> 1) & 1;
                uint32_t d2 = (load2 >> 2) & 1;
                uint32_t xor2 = a2 ^ b2;
                uint32_t xora2 = xor2 & a2;
                uint32_t xorb2 = xor2 & b2;
                uint32_t andnotb2 = xorb2 & (!ctemp2); 
                results[c2] |= andnotb2;
                temps[c2] = ctemp2 | xora2;
                sums[c2] += (d2 << (31 - j));

                //todid
                
                uint32_t a3 = (load2 >> (num_features)) & 1;
                uint32_t b3 = (load2 >> (num_features + 1)) & 1;
                uint32_t d3 = (load2 >> (num_features + 2)) & 1;
                uint32_t xor3 = a3 ^ b3;
                uint32_t xora3 = xor3 & a3;
                uint32_t xorb3 = xor3 & b3;
                uint32_t andnotb3 = xorb3 & (!ctemp3); 
                results[c2 + 1] |= andnotb3;
                temps[c2 + 1] = ctemp3 | xora3;
                sums[c2+1] += (d3 << (31 - j));
                //todid
                
                cres_idx += 4; // + 2 words worth of 2 samples per word
                data_index += 2;
            }
        }
        //todid

        res_idx += samples_per_block;
    }
        
        
    // goal here: UNROLL for every word of the cacheline so we don't keep load ing results and temps
    } else if(samples_per_word == 1){
        //TODO

    size_t res_idx = 0; // counts for k loop in samples
    size_t data_index = 0; // counts k loop in words
    for(int k = 0; k < num_blocks;k++){
        // load all res / temps -> don't need to load initially!
        uint32_t res0 = 0;
        uint32_t res1 = 0;
        uint32_t res2 = 0;
        uint32_t res3 = 0;
        uint32_t res4 = 0;
        uint32_t res5 = 0;
        uint32_t res6 = 0;
        uint32_t res7 = 0;
        
        uint32_t res8 = 0;
        uint32_t res9 = 0;
        uint32_t res10 = 0;
        uint32_t res11 = 0;
        uint32_t res12 = 0;
        uint32_t res13 = 0;
        uint32_t res14 = 0;
        uint32_t res15 = 0;
        
        uint32_t temp0 = 0;
        uint32_t temp1 = 0;
        uint32_t temp2 = 0;
        uint32_t temp3 = 0;
        uint32_t temp4 = 0;
        uint32_t temp5 = 0;
        uint32_t temp6 = 0;
        uint32_t temp7 = 0;
        
        uint32_t temp8 = 0;
        uint32_t temp9 = 0;
        uint32_t temp10 = 0;
        uint32_t temp11 = 0;
        uint32_t temp12 = 0;
        uint32_t temp13 = 0;
        uint32_t temp14 = 0;
        uint32_t temp15 = 0;
        
        uint32_t sum0 = 0;
        uint32_t sum1 = 0;
        uint32_t sum2 = 0;
        uint32_t sum3 = 0;
        uint32_t sum4 = 0;
        uint32_t sum5 = 0;
        uint32_t sum6 = 0;
        uint32_t sum7 = 0;
        
        uint32_t sum8 = 0;
        uint32_t sum9 = 0;
        uint32_t sum10 = 0;
        uint32_t sum11 = 0;
        uint32_t sum12 = 0;
        uint32_t sum13 = 0;
        uint32_t sum14 = 0;
        uint32_t sum15 = 0;



        for(int j = 0; j < word_size;++j){
            uint32_t load0 = data[data_index];
            uint32_t load1 = data[data_index + 1];
            uint32_t load2 = data[data_index + 2];
            uint32_t load3 = data[data_index + 3];
            uint32_t load4 = data[data_index + 4];
            uint32_t load5 = data[data_index + 5];
            uint32_t load6 = data[data_index + 6];
            uint32_t load7 = data[data_index + 7];
            
            uint32_t load8 = data[data_index + 8];
            uint32_t load9 = data[data_index + 9];
            uint32_t load10 = data[data_index + 10];
            uint32_t load11 = data[data_index + 11];
            uint32_t load12 = data[data_index + 12];
            uint32_t load13 = data[data_index + 13];
            uint32_t load14 = data[data_index + 14];
            uint32_t load15 = data[data_index + 15];
            
            uint32_t a0 = load0 & 1;
            uint32_t b0 = (load0 >> 1) & 1;
            uint32_t d0 = (load0 >> 2) & 1;
            uint32_t xor0 = a0 ^ b0;
            uint32_t xora0 = xor0 & a0;
            uint32_t xorb0 = xor0 & b0;
            uint32_t andnotb0 = xorb0 & (!temp0); 
            res0 |= andnotb0;
            temp0 = temp0 | xora0;
            sum0 += (d0 << (31 - j));

            
            uint32_t a1 = load1 & 1;
            uint32_t b1 = (load1 >> 1) & 1;
            uint32_t d1 = (load1 >> 2) & 1;            
            uint32_t xor1 = a1 ^ b1;
            uint32_t xora1 = xor1 & a1;
            uint32_t xorb1 = xor1 & b1;
            uint32_t andnotb1 = xorb1 & (!temp1); 
            res1 |= andnotb1;
            sum1 += (d1 << (31 - j));
            
            uint32_t a2 = load2 & 1;
            uint32_t b2 = (load2 >> 1) & 1;
            uint32_t d2 = (load2 >> 2) & 1;
            uint32_t xor2 = a2 ^ b2;
            uint32_t xora2 = xor2 & a2;
            uint32_t xorb2 = xor2 & b2;
            uint32_t andnotb2 = xorb2 & (!temp2); 
            res2 |= andnotb2;
            temp2 = temp2 | xora2;
            sum2 += (d2 << (31 - j));
            

            uint32_t a3 = load3 & 1;
            uint32_t b3 = (load3 >> 1) & 1;
            uint32_t d3 = (load3 >> 2) & 1;            
            uint32_t xor3 = a3 ^ b3;
            uint32_t xora3 = xor3 & a3;
            uint32_t xorb3 = xor3 & b3;
            uint32_t andnotb3 = xorb3 & (!temp3); 
            res3 |= andnotb3;
            temp3 = temp3 | xora3;
            sum3 += (d3 << (31 - j));
            
            uint32_t a4 = load4 & 1;
            uint32_t b4 = (load4 >> 1) & 1;
            uint32_t d4 = (load4 >> 2) & 1;            
            uint32_t xor4 = a4 ^ b4;
            uint32_t xora4 = xor4 & a4;
            uint32_t xorb4 = xor4 & b4;
            uint32_t andnotb4 = xorb4 & (!temp4); 
            res4 |= andnotb4;
            temp4 = temp4 | xora4;
            sum4 += (d4 << (31 - j));

            
            uint32_t a5 = load5 & 1;
            uint32_t b5 = (load5 >> 1) & 1;
            uint32_t d5 = (load5 >> 2) & 1;            
            uint32_t xor5 = a5 ^ b5;
            uint32_t xora5 = xor5 & a5;
            uint32_t xorb5 = xor5 & b5;
            uint32_t andnotb5 = xorb5 & (!temp5); 
            res5 |= andnotb5;
            temp5 = temp5 | xora5;
            sum5 += (d5 << (31 - j));
            
            uint32_t a6 = load6 & 1;
            uint32_t b6 = (load6 >> 1) & 1;
            uint32_t d6 = (load6 >> 2) & 1;
            uint32_t xor6 = a6 ^ b6;
            uint32_t xora6 = xor6 & a6;
            uint32_t xorb6 = xor6 & b6;
            uint32_t andnotb6 = xorb6 & (!temp6); 
            res6 |= andnotb6;
            temp6 = temp6 | xora6;
            sum6 += (d6 << (31 - j));
            
            uint32_t a7 = load7 & 1;
            uint32_t b7 = (load7 >> 1) & 1;
            uint32_t d7 = (load7 >> 2) & 1;
            uint32_t xor7 = a7 ^ b7;
            uint32_t xora7 = xor7 & a7;
            uint32_t xorb7 = xor7 & b7;
            uint32_t andnotb7 = xorb7 & (!temp7); 
            res7 |= andnotb7;
            temp7 = temp7 | xora7;
            sum7 += (d7 << (31 - j));
            
            
            uint32_t a8 = load8 & 1;
            uint32_t b8 = (load8 >> 1) & 1;
            uint32_t d8 = (load8 >> 2) & 1;
            uint32_t xor8 = a8 ^ b8;
            uint32_t xora8 = xor8 & a8;
            uint32_t xorb8 = xor8 & b8;
            uint32_t andnotb8 = xorb8 & (!temp8); 
            res8 |= andnotb8;
            temp8 = temp8 | xora8;
            sum8 += (d8 << (31 - j));

            
            uint32_t a9 = load9 & 1;
            uint32_t b9 = (load9 >> 1) & 1;
            uint32_t d9 = (load9 >> 2) & 1;
            uint32_t xor9 = a9 ^ b9;
            uint32_t xora9 = xor9 & a9;
            uint32_t xorb9 = xor9 & b9;
            uint32_t andnotb9 = xorb9 & (!temp9); 
            res9 |= andnotb9;
            temp9 = temp9 | xora9;
            sum9 += (d9 << (31 - j));

            
            uint32_t a10 = load10 & 1;
            uint32_t b10 = (load10 >> 1) & 1;
            uint32_t d10 = (load10 >> 2) & 1;
            uint32_t xor10 = a10 ^ b10;
            uint32_t xora10 = xor10 & a10;
            uint32_t xorb10 = xor10 & b10;
            uint32_t andnotb10 = xorb10 & (!temp10); 
            res10 |= andnotb10;
            temp10 = temp10 | xora10;
            sum10 += (d10 << (31 - j));
            
            uint32_t a11 = load11 & 1;
            uint32_t b11 = (load11 >> 1) & 1;
            uint32_t d11 = (load11 >> 2) & 1;
            uint32_t xor11 = a11 ^ b11;
            uint32_t xora11 = xor11 & a11;
            uint32_t xorb11 = xor11 & b11;
            uint32_t andnotb11 = xorb11 & (!temp11); 
            res11 |= andnotb11;
            temp11 = temp11 | xora11;
            sum11 += (d11 << (31 - j));
            
            uint32_t a12 = load12 & 1;
            uint32_t b12 = (load12 >> 1) & 1;
            uint32_t d12 = (load12 >> 2) & 1;
            uint32_t xor12 = a12 ^ b12;
            uint32_t xora12 = xor12 & a12;
            uint32_t xorb12 = xor12 & b12;
            uint32_t andnotb12 = xorb12 & (!temp12); 
            res12 |= andnotb12;
            temp12 = temp12 | xora12;
            sum12 += (d12 << (31 - j));
            
            uint32_t a13 = load13 & 1;
            uint32_t b13 = (load13 >> 1) & 1;
            uint32_t d13 = (load13 >> 2) & 1;
            uint32_t xor13 = a13 ^ b13;
            uint32_t xora13 = xor13 & a13;
            uint32_t xorb13 = xor13 & b13;
            uint32_t andnotb13 = xorb13 & (!temp13); 
            res13 |= andnotb13;
            temp13 = temp13 | xora13;
            sum13 += (d13 << (31 - j));
            
            uint32_t a14 = load14 & 1;
            uint32_t b14 = (load14 >> 1) & 1;
            uint32_t d14 = (load14 >> 2) & 1;            
            uint32_t xor14 = a14 ^ b14;
            uint32_t xora14 = xor14 & a14;
            uint32_t xorb14 = xor14 & b14;
            uint32_t andnotb14 = xorb14 & (!temp14); 
            res14 |= andnotb14;
            temp14 = temp14 | xora14;
            sum14 += (d14 << (31 - j));
            
            uint32_t a15 = load15 & 1;
            uint32_t b15 = (load15 >> 1) & 1;
            uint32_t d15 = (load15 >> 2) & 1;
            uint32_t xor15 = a15 ^ b15;
            uint32_t xora15 = xor15 & a15;
            uint32_t xorb15 = xor15 & b15;
            uint32_t andnotb15 = xorb15 & (!temp15); 
            res15 |= andnotb15;
            temp15 = temp15 | xora15;
            sum15 += (d15 << (31 - j));
            
            data_index += 16; //words per cacheline 
        }
        
        results[res_idx] = res0;
        results[res_idx + 1] = res1;
        results[res_idx + 2] = res2;
        results[res_idx + 3] = res3;
        results[res_idx + 4] = res4;
        results[res_idx + 5] = res5;
        results[res_idx + 6] = res6;
        results[res_idx + 7] = res7;

        results[res_idx + 8] = res8;
        results[res_idx + 9] = res9;
        results[res_idx + 10] = res10;
        results[res_idx + 11] = res11;
        results[res_idx + 12] = res12;
        results[res_idx + 13] = res13;
        results[res_idx + 14] = res14;
        results[res_idx + 15] = res15;
        
        sums[res_idx] += sum0;
        sums[res_idx + 1] += sum1;
        sums[res_idx + 2] += sum2;
        sums[res_idx + 3] += sum3;
        sums[res_idx + 4] += sum4;
        sums[res_idx + 5] += sum5;
        sums[res_idx + 6] += sum6;
        sums[res_idx + 7] += sum7;

        sums[res_idx + 8] += sum8;
        sums[res_idx + 9] += sum9;
        sums[res_idx + 10] += sum10;
        sums[res_idx + 11] += sum11;
        sums[res_idx + 12] += sum12;
        sums[res_idx + 13] += sum13;
        sums[res_idx + 14] += sum14;
        sums[res_idx + 15] += sum15;


        res_idx += 16; // samples per block

    }

    }

        //vectorized accumulation
        for(int i = 0; i <samples_per_block * num_blocks; i+=8){
            sumdata= (__m256i)_mm256_load_ps(sums+i);
            mask= _mm256_load_ps(results+i);
            mask =(__m256) _mm256_slli_epi32((__m256i)mask, 31);
            sumdata=(__m256i) _mm256_blendv_ps((__m256)zeros,(__m256)sumdata, mask);
            accum =  _mm256_add_epi32(accum, sumdata);
        }

        uint32_t accums[8];
        //memcpy(cres, &res1, sizeof(cres));
        _mm256_storeu_pd(accums,(__m256d) accum);
        sum += accums[0] + accums[1] + accums[2] + accums[3] + accums[4] + accums[5] + accums[6] + accums[7];
    
    return sum;

}

uint64_t q2_vector_weave(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size,int num_samples, int num_features,int number_entries){

    __m256i a1;
    __m256i b1;
    __m256i c1;
    __m256i xor1;
    __m256i a2;
    __m256i b2;
    __m256i c2;
    __m256i xor2;


    uint64_t csumtemp;
    uint64_t crestemp;    
    uint64_t sum = 0;

    __m256i res1 = _mm256_setzero_si256();
    __m256i res2 = _mm256_setzero_si256();

    
    __m256i temp1 = _mm256_setzero_si256();
    __m256i temp2 = _mm256_setzero_si256();
    
    
    __m256i sums1 = _mm256_setzero_si256();
    __m256i sums2 = _mm256_setzero_si256();

    __m256i c_mask = _mm256_setzero_si256(); 
    uint64_t csum_mask = 0;


    //case switch, masks for summing
    switch(num_features){
        case 2:
             c_mask = _mm256_set1_epi64x(0x5555555555555555); 
             csum_mask = 3;
            break; // really this case should call v3
        case 4:
             c_mask = _mm256_set1_epi64x(0x1111111111111111); 
             csum_mask = 15;
            break;//001100110011..0011 pattern
        case 8:
             c_mask = _mm256_set1_epi64x(0x0101010101010101); 
             csum_mask = 255;
            break; //0000001100000011 pattern
        case 16:
             c_mask = _mm256_set1_epi64x(0x0001000100010001); 
             csum_mask = 65535;
            break;
        case 32:
             c_mask = _mm256_set1_epi64x(0x0000000100000001); 
             csum_mask = 4294967295;
            break;
        case 64:
             c_mask = _mm256_set1_epi64x(0x0000000000000001);
             csum_mask = 18446744073709551615;
            break; // 30 0s, 2 1s
        default:
            printf("unexpected nr of features, returning \n");
            return;
    }



    int samples_per_block = 64 / num_features;
    int samples_per_cl = samples_per_block * 8;
    int num_cl = ceil(((float)num_samples) / samples_per_cl);
    
    uint64_t * d = data;
    int load_idx = 0;
    int res_idx = 0;
    
    for(int i = 0; i < num_cl; i++){    // cacheline block index -> 256 64bit words per cacheline block
        for(int j = 0; j < 32; j++){    // 32bit words -> 8 64bit words per cacheline (i.e. 256 samples bits)
            
            //UNROLL LOOP FOR BOTH VECTORS, just easier than an array of vectors or something
            // load correct vector
            //__m256i a0 = (__m256i) _mm256_loadu_pd(d + (i * 256) + j * 8);
            a1 = (__m256i) _mm256_load_pd(d + load_idx);
            /*
            if(i == 0){
                int64_t v64val0[4];
                memcpy(v64val0, &a0, sizeof(v64val0));
                PRINT_64_B(v64val0[0]);
                LINE;
                int64_t v64val[4];
                memcpy(v64val, &a1, sizeof(v64val));
                PRINT_64_B(v64val[0]);
                LINE;
                LINE;
                LINE;
            }
            */
            
            // shift right by one
            b1 = _mm256_srli_epi64(a1, 1);
            // TODO third column 
            c1 = _mm256_srli_epi64(a1, 2);
            //TODO mask out depending on num_features. Currently for 32
            c1 = _mm256_and_si256(c_mask, c1);
            //xor 
            xor1 = _mm256_xor_si256(a1, b1);
            
            //compute res
            b1 = _mm256_and_si256 (xor1, b1); //xor & b
            b1 = _mm256_andnot_si256 (temp1, b1); //(xor & b) & (~temp)
            res1 = _mm256_or_si256 (res1, b1); // res |= ^
            
            //compute temp
            a1 = _mm256_and_si256 (a1, xor1);
            temp1 = _mm256_or_si256 (temp1, a1);
            
            //sum buffer 1 
            //TODO handle different num_features
            sums1 = _mm256_or_si256 (sums1,_mm256_slli_epi64(c1, num_features-1-j));


            //now for second vector of the cache line
            // load correct vector
            a2 = (__m256i) _mm256_load_ps(d + load_idx + 4); // + 4 as 4 64 bit words per vector
            
            // shift right by one
            b2 = _mm256_srli_epi64(a2, 1);
            // TODO third column 
            c2 = _mm256_srli_epi64(a2, 2);
            c2 = _mm256_and_si256(c_mask, c2);
            //TODO mask depending on num_features. Currently for 32

            //xor 
            xor2 = _mm256_xor_si256(a2, b2);
            
            //compute res
            b2 = _mm256_and_si256 (xor2, b2); //xor & b
            b2 = _mm256_andnot_si256 (temp2, b2); //(xor & b) & (~temp)
            res2 = _mm256_or_si256 (res2, b2); // res |= ^
            
            //compute temp
            a2 = _mm256_and_si256 (a2, xor2);
            temp2 = _mm256_or_si256 (temp2, a2);
            
            //sum buffer 2
            sums2 = _mm256_or_si256(sums2, _mm256_slli_epi64(c2, num_features-1-j));


            load_idx += 8;
            
        }
        
        //printf("reach here");
        // read results out 
        uint64_t cres[4];
        //memcpy(cres, &res1, sizeof(cres));
        _mm256_storeu_pd(cres,(__m256d) res1); // need storeu so it doesn't crash (investigate?)
        
        uint64_t cres0 = cres[0];
        uint64_t cres1 = cres[1];
        uint64_t cres2 = cres[2];
        uint64_t cres3 = cres[3];

//TODO
        uint64_t csum[4];
        //memcpy(cres, &res1, sizeof(cres));
        _mm256_storeu_pd(csum,(__m256d) sums1); // need storeu so it doesn't crash (investigate?)
        //

        uint64_t csum0 = csum[0];
        uint64_t csum1 = csum[1];
        uint64_t csum2 = csum[2];
        uint64_t csum3 = csum[3];


        /*
        if(i == 0){
            printf("c0, c1, c2, c3 (in order): \n");
            PRINT_64_B(cres[0]);
            LINE;
            PRINT_64_B(cres[1]);
            LINE;
            PRINT_64_B(cres[2]);
            LINE;
            PRINT_64_B(cres[3]);
            LINE;
        }
        */

        // maybe only do this if there are > 1 samples per block? maybe > x  until its worth it?
        int ri0 = res_idx;
        int ri1 = res_idx + samples_per_block;
        int ri2 = res_idx + 2 * samples_per_block;
        int ri3 = res_idx + 3 * samples_per_block;
        int ri4 = res_idx + 4 * samples_per_block; // hopefully this is precomputed


        for(int m = 0; m < samples_per_block; m++){
            //todo do the csum


            // first 64 bit block
            results[ri0] = cres0 & 1;
            //mask out the other samples in the same word
            csumtemp = csum0 & csum_mask;
            if ((cres0 & 1) == 1){
                //todo mask csum0
                sum += csumtemp;
                //printf("csumptemp %d\t  csum0%d \t", csumtemp, csum0);
            }
            cres0 = cres0 >> num_features;
            csum0 = csum0 >> num_features;

            // second 64 bit block
            results[ri1] = cres1 & 1;
            csumtemp = csum1 & csum_mask;
            if ((cres1 & 1) == 1){
                //todo mask csum0
                sum+= csumtemp;
            }
            cres1 = cres1 >> num_features;
            csum1 = csum1 >> num_features;
            
            //third
            results[ri2] = cres2 & 1;
            csumtemp = csum2 & csum_mask;
            if ((cres2 & 1) == 1){
                //todo mask csum0
                sum+= csumtemp;
            }
            cres2 = cres2 >> num_features;
            csum2 = csum2 >> num_features;
            
            
            //fourth
            results[ri3] = cres3 & 1;
            csumtemp = csum3 & csum_mask;
            if ((cres3 & 1) == 1){
                //todo mask csum0
                sum+= csumtemp;
            }
            cres3 = cres3 >> num_features;
            csum3 = csum3 >> num_features;

            
            ri0++;
            ri1++;
            ri2++;
            ri3++;
        }
        
        // NOW SAME THING BUT FOR res2 !
        
        // read results out 
        uint64_t cres_2[4];
        _mm256_storeu_pd(cres_2,(__m256d) res2);
        //memcpy(cres_2, &res2, sizeof(cres_2));
        uint64_t cres02 = cres_2[0];
        uint64_t cres12 = cres_2[1];
        uint64_t cres22 = cres_2[2];
        uint64_t cres32 = cres_2[3];

        // read results out 
        uint64_t csum_2[4];
        _mm256_storeu_pd(csum_2,(__m256d) sums2);
        //memcpy(csum_2, &res2, sizeof(csum_2));
        uint64_t csum02 = csum_2[0];
        uint64_t csum12 = csum_2[1];
        uint64_t csum22 = csum_2[2];
        uint64_t csum32 = csum_2[3];

        int ri5 = ri4 + samples_per_block;
        int ri6 = ri4 + 2 * samples_per_block;
        int ri7 = ri4 + 3 * samples_per_block;
        for(int m = 0; m < samples_per_block; m++){
            // first 64 bit block
            results[ri4] = cres02 & 1;
            csumtemp = csum02 & csum_mask;
            if ((cres02 & 1) == 1){
                //todo mask csum0
                sum+= csumtemp;
            }
            cres02 = cres02 >> num_features;
            csum02 = csum02 >> num_features;
            
            // second 64 bit block
            results[ri5] = cres12 & 1;
            csumtemp = csum12 & csum_mask;
            if ((cres12 & 1) == 1){
                //todo mask csum0
                sum+= csumtemp;
            }
            cres12 = cres12 >> num_features;
            csum12 = csum12 >> num_features;
            
            //third
            results[ri6] = cres22 & 1;
            csumtemp = csum22 & csum_mask;
            if ((cres22 & 1) == 1){
                //todo mask csum0
                sum+= csumtemp;
            }
            cres22 = cres22 >> num_features;
            csum22 = csum22 >> num_features;
            
            
            //fourth
            results[ri7] = cres32 & 1;
            csumtemp = csum32 & csum_mask;
            if ((cres32 & 1) == 1){
                //todo mask csum0
                sum+= csumtemp;
            }
            cres32 = cres32 >> num_features;
            csum32 = csum32 >> num_features;

            //printf("SAMPLE NR: %i \n", i * samples_per_cl + (4 + 3) * samples_per_block + m);
            ri4++;
            ri5++;
            ri6++;
            ri7++;
        }
        
        res_idx += samples_per_cl;
        
        // reset temp and res
        res1 = _mm256_setzero_si256();
        res2 = _mm256_setzero_si256();
        
        temp1 = _mm256_setzero_si256();
        temp2 = _mm256_setzero_si256();

        sums1 = _mm256_setzero_si256();
        sums2 = _mm256_setzero_si256();
    }
    return sum;
}

uint64_t q2_vector_weave_2(uint32_t * data,uint32_t * results,uint32_t *temps, uint32_t *sums,int word_size,int block_size,int num_samples, int num_features,int number_entries){

    __m256i a1;
    __m256i b1;
    __m256i c1;
    __m256i xor1;
    __m256i a2;
    __m256i b2;
    __m256i c2;
    __m256i xor2;

    __m256i zeros= _mm256_setzero_si256();
    __m256i accum= _mm256_setzero_si256();
    __m256 mask;

    uint64_t csumtemp;
    uint64_t crestemp;    
    uint64_t sum = 0;

    __m256i res1 = _mm256_setzero_si256();
    __m256i res2 = _mm256_setzero_si256();

    
    __m256i temp1 = _mm256_setzero_si256();
    __m256i temp2 = _mm256_setzero_si256();
    
    
    __m256i sums1 = _mm256_setzero_si256();
    __m256i sums2 = _mm256_setzero_si256();

    __m256i c_mask = _mm256_set1_epi64x(0x0000000100000001); 
    uint64_t csum_mask = 4294967295;

    int samples_per_block = 64 / num_features;
    int samples_per_cl = samples_per_block * 8;
    int num_cl = ceil(((float)num_samples) / samples_per_cl);
    
    uint64_t * d = data;
    int load_idx = 0;
    int res_idx = 0;
    
    for(int i = 0; i < num_cl; i++){    // cacheline block index -> 256 64bit words per cacheline block
        for(int j = 0; j < 32; j++){    // 32bit words -> 8 64bit words per cacheline (i.e. 256 samples bits)
            
            //UNROLL LOOP FOR BOTH VECTORS, just easier than an array of vectors or something
            // load correct vector
            //__m256i a0 = (__m256i) _mm256_loadu_pd(d + (i * 256) + j * 8);
            a1 = (__m256i) _mm256_load_pd(d + load_idx);
            /*
            if(i == 0){
                int64_t v64val0[4];
                memcpy(v64val0, &a0, sizeof(v64val0));
                PRINT_64_B(v64val0[0]);
                LINE;
                int64_t v64val[4];
                memcpy(v64val, &a1, sizeof(v64val));
                PRINT_64_B(v64val[0]);
                LINE;
                LINE;
                LINE;
            }
            */
            
            // shift right by one
            b1 = _mm256_srli_epi64(a1, 1);
            // TODO third column 
            c1 = _mm256_srli_epi64(a1, 2);
            //TODO mask out depending on num_features. Currently for 32
            c1 = _mm256_and_si256(c_mask, c1);
            //xor 
            xor1 = _mm256_xor_si256(a1, b1);
            
            //compute res
            b1 = _mm256_and_si256 (xor1, b1); //xor & b
            b1 = _mm256_andnot_si256 (temp1, b1); //(xor & b) & (~temp)
            res1 = _mm256_or_si256 (res1, b1); // res |= ^
            
            //compute temp
            a1 = _mm256_and_si256 (a1, xor1);
            temp1 = _mm256_or_si256 (temp1, a1);
            
            //sum buffer 1 
            //TODO handle different num_features
            sums1 = _mm256_or_si256 (sums1,_mm256_slli_epi64(c1, num_features-1-j));


            //now for second vector of the cache line
            // load correct vector
            a2 = (__m256i) _mm256_load_ps(d + load_idx + 4); // + 4 as 4 64 bit words per vector
            
            // shift right by one
            b2 = _mm256_srli_epi64(a2, 1);
            // TODO third column 
            c2 = _mm256_srli_epi64(a2, 2);
            c2 = _mm256_and_si256(c_mask, c2);
            //TODO mask depending on num_features. Currently for 32

            //xor 
            xor2 = _mm256_xor_si256(a2, b2);
            
            //compute res
            b2 = _mm256_and_si256 (xor2, b2); //xor & b
            b2 = _mm256_andnot_si256 (temp2, b2); //(xor & b) & (~temp)
            res2 = _mm256_or_si256 (res2, b2); // res |= ^
            
            //compute temp
            a2 = _mm256_and_si256 (a2, xor2);
            temp2 = _mm256_or_si256 (temp2, a2);
            
            //sum buffer 2
            sums2 = _mm256_or_si256(sums2, _mm256_slli_epi64(c2, num_features-1-j));


            load_idx += 8;
            
        }
        
        // read results out 
        uint64_t cres[4];
        //memcpy(cres, &res1, sizeof(cres));
        _mm256_storeu_pd(cres,(__m256d) res1); // need storeu so it doesn't crash (investigate?)
        
        uint64_t cres0 = cres[0];
        uint64_t cres1 = cres[1];
        uint64_t cres2 = cres[2];
        uint64_t cres3 = cres[3];

//TOD


        /*
        if(i == 0){
            printf("c0, c1, c2, c3 (in order): \n");
            PRINT_64_B(cres[0]);
            LINE;
            PRINT_64_B(cres[1]);
            LINE;
            PRINT_64_B(cres[2]);
            LINE;
            PRINT_64_B(cres[3]);
            LINE;
        }
        */

        // maybe only do this if there are > 1 samples per block? maybe > x  until its worth it?
        int ri0 = res_idx;
        int ri1 = res_idx + samples_per_block;
        int ri2 = res_idx + 2 * samples_per_block;
        int ri3 = res_idx + 3 * samples_per_block;
        int ri4 = res_idx + 4 * samples_per_block; // hopefully this is precomputed


        for(int m = 0; m < samples_per_block; m++){
            //todo do the csum


            // first 64 bit block
            results[ri0] = cres0 & 1;
            //mask out the other samples in the same word

            cres0 = cres0 >> num_features;

            // second 64 bit block
            results[ri1] = cres1 & 1;

            cres1 = cres1 >> num_features;

            //third
            results[ri2] = cres2 & 1;

            cres2 = cres2 >> num_features;
          //fourth
            results[ri3] = cres3 & 1;

            cres3 = cres3 >> num_features;

            
            ri0++;
            ri1++;
            ri2++;
            ri3++;
        }

        mask= _mm256_loadu_ps(results+res_idx);

        mask =(__m256) _mm256_slli_epi32((__m256i)mask, 31);
        sums1=(__m256i) _mm256_blendv_ps((__m256)zeros,(__m256)sums1, mask);
        accum =  _mm256_add_epi32(accum, sums1);
        
        // NOW SAME THING BUT FOR res2 !
        
        // read results out 
        uint64_t cres_2[4];
        _mm256_storeu_pd(cres_2,(__m256d) res2);
        //memcpy(cres_2, &res2, sizeof(cres_2));
        uint64_t cres02 = cres_2[0];
        uint64_t cres12 = cres_2[1];
        uint64_t cres22 = cres_2[2];
        uint64_t cres32 = cres_2[3];



        int ri5 = ri4 + samples_per_block;
        int ri6 = ri4 + 2 * samples_per_block;
        int ri7 = ri4 + 3 * samples_per_block;
        for(int m = 0; m < samples_per_block; m++){
            // first 64 bit block
            results[ri4] = cres02 & 1;

            cres02 = cres02 >> num_features;
            
            // second 64 bit block
            results[ri5] = cres12 & 1;

            cres12 = cres12 >> num_features;
            
            //third
            results[ri6] = cres22 & 1;

            cres22 = cres22 >> num_features;
            
            
            //fourth
            results[ri7] = cres32 & 1;

            cres32 = cres32 >> num_features;

            //printf("SAMPLE NR: %i \n", i * samples_per_cl + (4 + 3) * samples_per_block + m);
            ri4++;
            ri5++;
            ri6++;
            ri7++;
        }
        
        mask= _mm256_loadu_ps(results+ res_idx + 4 * samples_per_block);
        mask =(__m256) _mm256_slli_epi32((__m256i)mask, 31);
        sums2=(__m256i) _mm256_blendv_ps((__m256)zeros,(__m256)sums2, mask);
        accum = _mm256_add_epi32(accum, sums2);


        res_idx += samples_per_cl;
        
        // reset temp and res
        res1 = _mm256_setzero_si256();
        res2 = _mm256_setzero_si256();
        
        temp1 = _mm256_setzero_si256();
        temp2 = _mm256_setzero_si256();

        sums1 = _mm256_setzero_si256();
        sums2 = _mm256_setzero_si256();
    }


    //vectorized accumulation
    uint32_t accums[8];
    //memcpy(cres, &res1, sizeof(cres));
    _mm256_storeu_pd(accums,(__m256d) accum);
    sum += accums[0] + accums[1] + accums[2] + accums[3] + accums[4] + accums[5] + accums[6] + accums[7];
    

    return sum;
}