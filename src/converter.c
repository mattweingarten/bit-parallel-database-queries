/*
With some code adapted from mlweaving.h



*/
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/debug.h"
#include "../include/converter.h"
#define BATCH_BITS 64
#define BITS_OF_CL 512



/*
Destination just an array of samples w/ features
Stored such that we have each sample in its entirety before the next
[0a, 0b, 0c, 0d, 1a, 1b, 1c, 1d, 2a, ...]
Nr of Samples * Features % 256 == 0

Choosing values so expected MLWeave 32bit word is 01100110 x4

NOTE: must malloc before calling this
Nr of features should be a multiple of 4
Return -1 on failure, 0 on success
*/

char show_debug = 0;

int create_naive_example_samples(uint32_t *dest, uint32_t numSamples, uint32_t numFeatures){
	if((numSamples * numFeatures) % 256 != 0){
		printf("Simple sample array doesn't align nicely, should choose numSamples / numFeatures s.t. they align w/ 256b. \n Current values: \n numSamples: %u , numFeatures: %u \n\n", numSamples, numFeatures);
		return -1;
	}
	for(uint32_t i = 0; i < numSamples; i++){
		for(uint32_t j = 0; j < numFeatures; j += 4){
			dest[(i * numFeatures) + j] = 0 + j;
			dest[(i * numFeatures) + j + 1] = 4294967295u;
			dest[(i * numFeatures) + j + 2] = 4294967295u;
			dest[(i * numFeatures) + j + 3] = 0 + j;				
		}
	}
	
	return 0;
}

int create_example_samples(uint32_t *dest, uint32_t numSamples, uint32_t numFeatures){
	for(uint32_t i = 0; i < numSamples; i++){
		for(uint32_t j = 0; j < numFeatures; j++)
		dest[i * numFeatures + j] = i;
	}
	return 0;
}

/*
Cache line is 512bits
so per line we store 512 features' single bits
since we are only interested in "neighbouring" features, I will do 4 features per sample per cache line
(TODO:: Make this more modular)
*/
int weave_samples_simple(uint32_t *dest, uint32_t *src, uint32_t numSamples, uint32_t numFeatures){
	uint32_t address_index = 0;
	
	if(numFeatures > 64){
		printf("Too many features for a simple weaving! MAX == 64 \n\n");
		return -1;
	}
	else if(512 % numFeatures != 0){
		printf("Features don't go into 512 evenly\n\n");
		return -1;
	}
	
	// 512 bits of CL must be evenly divisible by numFeatures
	uint32_t samples_in_CL = BITS_OF_CL / numFeatures;
	
	
	for(uint32_t i = 0; i < numSamples; i += samples_in_CL){
		// the j loop from ML weaving can be added once we do more than 64 features per sample (or we grow the number of features per sample per cache line)
		
		uint32_t tmp_buffer[512] = {0};
		for(uint32_t k = 0; k < samples_in_CL; k++){
			for(uint32_t m = 0; m < numFeatures; m++){
				tmp_buffer[k * numFeatures + m] = src[(i + k) * numFeatures + m]; //for now just a straightforward copy, can adapt to mimick their loop once more modular
			}
		}
		
		// k loops through the 32 bits of each value, its effect is felt by the line: tmp_buffer[m] = tmp_buffer[m] << 1
		for (int k = 0; k < 32; k++){	
				uint32_t result_buffer[16] = {0};
				//2.1: re-order the data according to the bit-level...
				/*
				For each bit of the cacheline we:
				e.g. m = 0:
				result_buffer[0] = 0 | most significant bit of tmp_buffer[0] << 0 -> so least significant bit is first sample first feature first bit
				now result_buffer[0] = 00..000
				THEN SHIFT tmp_buffer[0] one to the left (so MSB is now new) 
				m = 1:
				result_buffer[0] = 00..000 | MSB of tmp_buffer[1] << 1
				now result_buffer[0] = 00..010 
				FIRST bit of first 32 values into result buffer 0
				
				m = 32:
				
				result_buffer[1] = result_buffer[1] | tmp_buffer[32] msb etcetc
				
				SO: temp_buffer should have the shape:
				a0 a1 a2 a3 b0 b1 b2 b3 ... h0 h1 h2 h3
				a4 a5 a6 a7 b4 b5 b6 b7 ...
				
				*/
				for (int m = 0; m < BITS_OF_CL; m++)
				{
					result_buffer[m>>5] = result_buffer[m>>5] | ((tmp_buffer[m] >>31)<<(m&31));
					tmp_buffer[m]       = tmp_buffer[m] << 1;				
				}
			    //2.2: store the bit-level result back to the memory...
				dest[address_index++] = result_buffer[0];
				dest[address_index++] = result_buffer[1];
				dest[address_index++] = result_buffer[2];
				dest[address_index++] = result_buffer[3];
				dest[address_index++] = result_buffer[4];
				dest[address_index++] = result_buffer[5];
				dest[address_index++] = result_buffer[6];
				dest[address_index++] = result_buffer[7];
				dest[address_index++] = result_buffer[8];
				dest[address_index++] = result_buffer[9];
				dest[address_index++] = result_buffer[10];
				dest[address_index++] = result_buffer[11];
				dest[address_index++] = result_buffer[12];
				dest[address_index++] = result_buffer[13];
				dest[address_index++] = result_buffer[14];
				dest[address_index++] = result_buffer[15];
				
				if(i == 0 && show_debug){
					if(k == 0) printf("Mem address of m: %p \n", &dest[2]);
					uint32_t m = result_buffer[1];
					printf("m: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(m>>24), BYTE_TO_BINARY(m>>16), BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));					
				}
				
			
		}
		//printf("\n");
	}
	return 0;
}

/*
Hopefully able to weave more than 64 features by moving them into another row (i.e. 32 values further along in the array)

Hopefully able to round the number of features and samples somewhat

STILL VERY MUCH UNFINISHED, maybe base off of simple and start over
*/
/*
void weave_samples(uint32_t *dest, uint32_t *src, uint32_t numSamples, uint32_t numFeatures){
	uint32_t address_index = 0;
	
	// note: numFeatures should be padded such that it divides 512 (i.e. a power of 2)
	uint32_t samples_in_CL = 512 / numFeatures;
	
	if(numFeatures > 64)
		samples_in_CL = 512 / 64; // don't want to cross 128 line boundaries when loading (?) -> just using 64 as in the paper for now
	
		
	for(uint32_t i = 0; i < numSamples; i += samples_in_CL){
		
		uint32_t samples_in_batch = ( (i+samples_in_CL)<numSamples )? samples_in_CL:(numSamples-i); 
		
		for (uint32_t j = 0; j < numFeatures; j += BATCH_BITS){
			uint32_t tmp_buffer[512] = {0};
			// split into 8 "batches" à 64 bits 
			for(uint32_t k = 0; j < samples_in_CL; j++){
				for(uint32_t m = 0; k < numFeatures; k++){
					tmp_buffer[j * numFeatures + k] = src[(i + j) * numFeatures + k]; //for now just a straightforward copy, can adapt to mimick their loop once more modular / more than 4 features per sample
				}
			}
			
			// k loops through the 32 bits of each value, its effect is felt by the line: tmp_buffer[m] = tmp_buffer[m] << 1
			for (int k = 0; k < 32; k++){	
					uint32_t result_buffer[16] = {0};
					//2.1: re-order the data according to the bit-level...

					for (int m = 0; m < 512; m++)
					{
						result_buffer[m>>5] = result_buffer[m>>5] | ((tmp_buffer[m] >>31)<<(m&31));
						tmp_buffer[m]       = tmp_buffer[m] << 1;				
					}
					//2.2: store the bit-level result back to the memory...
					dest[address_index++] = result_buffer[0];
					dest[address_index++] = result_buffer[1];
					dest[address_index++] = result_buffer[2];
					dest[address_index++] = result_buffer[3];
					dest[address_index++] = result_buffer[4];
					dest[address_index++] = result_buffer[5];
					dest[address_index++] = result_buffer[6];
					dest[address_index++] = result_buffer[7];
					dest[address_index++] = result_buffer[8];
					dest[address_index++] = result_buffer[9];
					dest[address_index++] = result_buffer[10];
					dest[address_index++] = result_buffer[11];
					dest[address_index++] = result_buffer[12];
					dest[address_index++] = result_buffer[13];
					dest[address_index++] = result_buffer[14];
					dest[address_index++] = result_buffer[15];
				
				}
			
			
			
		}

	}
}
	
*/


// NOT CORRECT YET ! test index >= 128 gives us some sort of a mess im not sure why

	void retrieve_from_simple_mlweaving(uint32_t *dest, uint32_t *src, uint32_t index, uint32_t num_bits, uint32_t numFeatures) 
{		
	//calculate the address of sample of the index: index. 
	uint32_t  sample_offset     = ((index * numFeatures) / 32) % 16; //index * features / 32 % 16 tells us which word of a cacheline we're after
	// still need to add cache line
	uint32_t cl_offset 			= ((uint32_t)(index * numFeatures) / BITS_OF_CL) * 16 * 32; // 16 * 32 words converted per block
	
	uint32_t* sample_addr       = src + sample_offset + cl_offset; //identify the address of the sampel (index)

	uint32_t within_offset = (index % (32 / numFeatures)) * numFeatures; // round index down according to features per uint32_t, then make index by multiplying
	
	if(show_debug){
	printf("sample_offset: %u, within_offset: %u, index: %u \n", sample_offset, within_offset, index);
	
	printf("Memory address of src: %p , src + sample offset: %p , expected: %p \n\n", src, sample_addr);
	}
	uint32_t t_bits_min_1       = sizeof(uint32_t)*8 - 1;

	for (size_t i = 0; i < numFeatures; i++) 
	{  //prepare one number for each iteration.  
		uint32_t bit_offset  = (within_offset + i) & 31;

		//The next 32 CLs contains the information of the feature. 
		uint32_t result = 0;
		uint32_t tmp;
		for (uint32_t j = 0; j < num_bits; j++)
		{
					// sample address should be uint32_t representing first bit of these features
		  tmp	  = sample_addr[(BITS_OF_CL/32)*j ]; //jump by a cacheline per bit
		  if(i == 0 && show_debug){
			uint32_t m = tmp;
			printf("m: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(m>>24), BYTE_TO_BINARY(m>>16), BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));  
		  }
		  result |= ( ( (tmp&(1<<bit_offset)) >> bit_offset ) << (t_bits_min_1-j) ); //
		}
		printf("\n");
		dest[i] = result; 
	}
}


// int main(int argc, char **argv) {
	
// 	uint32_t numSamples = 2560;
// 	uint32_t numFeatures = 4;
// 	uint32_t test_index = 8;
// 	int j;
// 	for (j = 1; j < argc; j++) {
// 		int more = j+1 < argc; // There are more arguments

//         if (!strcmp(argv[j],"--nf") && more) { 		  // NUM FEATURES
//             numFeatures = (uint32_t) atof(argv[++j]);
// 		} else if (!strcmp(argv[j],"--ns") && more) { // NUM SAMPLES
// 			numSamples = (uint32_t) atof(argv[++j]);
// 		} else if (!strcmp(argv[j],"--ti") && more) { // TEST INDEX
// 			test_index = (uint32_t) atof(argv[++j]);
// 		} else if (!strcmp(argv[j],"--d")){			  // DEBUG
// 			show_debug = 1;
// 		} else {
//             printf("Unknown or not enough arguments for option '%s'.\n\n",
//                 argv[j]);
//             exit(1);
//         }
				
// 	}
	
// 	if(test_index >= numSamples){
// 		printf("Test index exceeds number of samples, be reasonable!\n\n");
// 		exit(1);
// 	} else if ((numSamples * numFeatures) % 512 != 0){
// 		printf("Test array doesn't align with cache line, will get odd behaviour towards end (TODO: PAD THE BUFFERS ACCORDINGLY ?). \n\n");
// 	}
	
// 	uint32_t* normal = (uint32_t*) malloc(numSamples * numFeatures * sizeof(uint32_t));
	
// 	uint32_t* weaved = (uint32_t*) malloc(numSamples * numFeatures * sizeof(uint32_t));
	
// 	uint32_t* check = (uint32_t*) malloc(numFeatures * sizeof(uint32_t));
	
// 	int res = 0;
	
// 	/*
// 	if(create_naive_example_samples(normal, numSamples, numFeatures) != 0){
// 		printf("Example sample creation failed \n");
// 		return 0;
// 	}
	
// 	res = weave_samples_simple(weaved, normal, numSamples, numFeatures);
	
// 	if(res != 0){
// 		printf("some sort of error in weaving\n\n");
// 		return 0;
// 	}
	
	
	
// 	retrieve_from_simple_mlweaving(check, weaved, test_index, 32, numFeatures);
	
// 	for(int i = 0; i < 4; i++)
// 		printf("feature %i: %u == %u ? \n", i, check[i], normal[test_index * numFeatures + i]);
	
// 	*/
// 	//numFeatures *= 2;
// 	//numSamples /= 2;
	
// 	create_example_samples(normal, numSamples, numFeatures);
	
// 	res = weave_samples_simple(weaved, normal, numSamples, numFeatures);
	
// 	if(res != 0){
// 		printf("some sort of error in weaving\n\n");
// 		return 0;
// 	}
	
// 	retrieve_from_simple_mlweaving(check, weaved, test_index, 32, numFeatures);
	
// 	for(int i = 0; i < numFeatures; i++)
// 		printf("feature %i: %u == %u ? \n", i, check[i], normal[test_index * numFeatures + i]);
	
// 	printf("Everything o k \n");
// 	return 0;
// }