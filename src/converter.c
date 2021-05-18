/*
With some code adapted from mlweaving.h



*/
#include <stdint.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/debug.h"
#include "../include/converter.h"
#define BLOCK_BITS 64
#define BITS_OF_CL 512



/*
Destination just an array of samples w/ features
Stored such that we have each sample in its entirety before the next
[0a, 0b, 0c, 0d, 1a, 1b, 1c, 1d, 2a, ...] (note: LSB is on the left here)
*/

//wrapper functions
uint32_t *weave_samples_wrapper(uint32_t* data,size_t rows,size_t cols){
    size_t numEntries = numberOfEntries(rows,cols);
    uint32_t * res = (uint32_t* ) aligned_alloc(32,numEntries * sizeof(uint32_t));
	weave_samples(res,data,rows,cols);
    return res;
}



char show_debug = 0;

int create_example_samples(uint32_t *dest, uint32_t numSamples, uint32_t numFeatures){
	//int count = 0;
	for(uint32_t i = 0; i < numSamples; i++){
		for(uint32_t j = 0; j < numFeatures; j++)
		dest[i * numFeatures + j] = i + j;
	}
	return 0;
}

/*
Cache line is 512bits
so per line we store 512 features' single bits
up to 64 features per sample so far

NOTE: Pads to 64 bit words!
*/
int weave_samples(uint32_t *dest, uint32_t *src, uint32_t numSamples, uint32_t numFeatures){
	uint32_t address_index = 0;
	
	if(numFeatures > 64){
		printf("Too many features for a weaving! MAX == 64 \n\n");
		return -1;
	}
	
	uint32_t total_values = numSamples * numFeatures;
	
	uint32_t samples_per_block = BLOCK_BITS / numFeatures;
	
	int blocks_per_CL = BITS_OF_CL / BLOCK_BITS; //(should always be 8), should define as constants.. later

	// rounded down! 
	uint32_t samples_per_CL = blocks_per_CL * samples_per_block;
	
	
	for(uint32_t i = 0; i < numSamples; i += samples_per_CL){
		// in case our cacheline doesn't contain 512 values (so we don't go out of bounds accessing src)
		uint32_t samples_in_CL = (i + samples_per_CL) < numSamples ? samples_per_CL : numSamples - i;
		
		uint32_t blocks_in_CL = ceil(((float)samples_in_CL) / samples_per_block);
		
		if(show_debug && (i == 0 || i + samples_per_CL >= numSamples))
			printf("samples in cacheline: %u \nblocks in CL: %u \nsamples per cacheline: %u \nsamples per block: %u \n\n", samples_in_CL, blocks_in_CL, samples_per_CL, samples_per_block);
		
		uint32_t tmp_buffer[512] = {0};
		for(uint32_t j = 0; j < blocks_in_CL; j++){
			// samples in this block is either max amount, or reduced
			uint32_t samples_in_block = (samples_in_CL - j * samples_per_block) > samples_per_block ? samples_per_block : samples_in_CL - j * samples_per_block;
			for(uint32_t k = 0; k < samples_in_block; k++){
				for(uint32_t m = 0; m < numFeatures; m++){
					
					// making them variables for readability
					
					// i => correct cacheline
					// j => correct block within cacheline
					// k => correct sample within block
					// m => correct feature within sample
					size_t load_index = (i + k + samples_per_block * j) * numFeatures + m;
					
					
					size_t store_index = j * BLOCK_BITS + k * numFeatures + m;
					
					//printf("load_index: %zu , store_index: %zu sib: %u \n", load_index, store_index, samples_in_block);
					tmp_buffer[store_index] = src[load_index]; 
				}
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
					int a = 3;
					int b = a - 1;
					if(k == 0) printf("Encoding, showing result_buffer[%i], [%i] ----------- \n", a, b);
					uint32_t m = result_buffer[b];
					printf("%i: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN, a, BYTE_TO_BINARY(m>>24), BYTE_TO_BINARY(m>>16), BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));
					m = result_buffer[a];
					printf(" %i: "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", b, BYTE_TO_BINARY(m>>24), BYTE_TO_BINARY(m>>16), BYTE_TO_BINARY(m>>8), BYTE_TO_BINARY(m));						
				}
				
			
		}
	}
	// printf("\n\n");
	return 0;
}

	void retrieve_from_simple_mlweaving(uint32_t *dest, uint32_t *src, uint32_t index, uint32_t num_bits, uint32_t numFeatures) 
{	
	// need these values again
	uint32_t samples_per_block = BLOCK_BITS / numFeatures;
	
	int blocks_per_CL = BITS_OF_CL / BLOCK_BITS; //(should always be 8), should define as constants.. later

	// rounded down! 
	uint32_t samples_per_CL = blocks_per_CL * samples_per_block;
	
	// tells us which CL we're in
	uint32_t cl_offset = (index / samples_per_CL) * 512;
	
	// gives us an index within the CL
	uint32_t cl_index = index % samples_per_CL;
	
	// tells us which bit within the block we start on
	int within_offset = (cl_index % samples_per_block) * numFeatures;
	
	// division tells us which block it is, * 2 to give us the word (first block is words 0, 1. second is 2, 3 etc)
	uint32_t word_offset = 2 * (cl_index / samples_per_block);
	
	// if in second word of block we have to add one to word offset
	word_offset = within_offset > 31 ? word_offset + 1 : word_offset;
	
	
	// tells us which bit of the word we start in
	within_offset = within_offset % 32; 
	
	
	uint32_t* sample_addr = src + word_offset + cl_offset; //identify the address of the sample (index)

	
	if(show_debug){
	printf("Retrieval of Weaved data ------------- \ncl_offset: %u, cl_idx: %u, word_offset: %u, within_offset: %i, index: %u \n", cl_offset, cl_index, word_offset, within_offset, index);
	
	printf("Memory address of src: %p , src + sample offset: %p \n\n", src, sample_addr);
	}
	uint32_t t_bits_min_1       = sizeof(uint32_t)*8 - 1;

	for (size_t i = 0; i < numFeatures; i++) 
	{  //prepare one number for each iteration.  
		// have to move to next word
		if(within_offset + i == 32){
			sample_addr++;
			within_offset = -i; // and make it so that bit_offset will be 0 for this value
		}
		
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
		dest[i] = result; 
	}
	printf("\n");
}

uint32_t numberOfEntries(uint32_t numSamples,	uint32_t numFeatures){
	uint32_t numEntries = numSamples * numFeatures;
	
	// num samples per 64bits (round down!) (ONE "BLOCK")
	int samples_per_block = 64 / numFeatures;
	if(samples_per_block * numFeatures != 64){		
		// how many 64bit blocks do we have?
		int numBlocks = ceil(((float)numSamples) / samples_per_block);
		
		// how many cachelines do we have? (always need all 512 to keep the indexing correct)
		int ceiling = ceil(numBlocks * 64 / 512.0);
		numEntries = ceiling * 512;
	}
	return numEntries;
}





int converter_test(uint32_t numSamples,	uint32_t numFeatures, uint32_t test_index) {
		
	// compute / pad number of entries
	uint32_t numEntries = numSamples * numFeatures;
	
	// num samples per 64bits (round down!) (ONE "BLOCK")
	int samples_per_block = 64 / numFeatures;
	
	// if it doesn't align perfectly, have to pad
	if(test_index >= numSamples){
		printf("Test index exceeds number of samples, be reasonable!\n\n");
		exit(1);
	} else if (samples_per_block == 0){
		printf("Maximum 64 features per sample exceeded!\n\n");
		exit(1);
	} else if(samples_per_block * numFeatures != 64){
		printf("Number of features doesn't align w/ 64 bit words, padding.\n");
		
		// how many 64bit blocks do we have?
		int numBlocks = ceil(((float)numSamples) / samples_per_block);
		
		// how many cachelines do we have? (always need all 512 to keep the indexing correct)
		int ceiling = ceil(numBlocks * 64 / 512.0);
		numEntries = ceiling * 512;
	}
	
	// allocate memory
	uint32_t* normal = (uint32_t*) malloc(numSamples * numFeatures * sizeof(uint32_t));
	
	uint32_t* weaved = (uint32_t*) malloc(numEntries * sizeof(uint32_t));
	
	uint32_t* check = (uint32_t*) malloc(numFeatures * sizeof(uint32_t));
	
	int res = 0;
	
	// create and weave example
	create_example_samples(normal, numSamples, numFeatures);
	
	res = weave_samples(weaved, normal, numSamples, numFeatures);
	
	if(res != 0){
		printf("some sort of error in weaving\n\n");
		return 0;
	}
	
	// retrieve 
	retrieve_from_simple_mlweaving(check, weaved, test_index, 32, numFeatures);
	
	printf("numSamples: %u ---- numFeatures: %u ---- numEntries: %u \n" 
	"Testing on sample with index = %u \n"
	"Retrieved == Expected ?\n", numSamples, numFeatures, numEntries, test_index);
	char flag = 0;
	for(int i = 0; i < numFeatures; i++){
		if(check[i] != normal[test_index * numFeatures + i]){
			printf("WARNING:: feature %i: %u == %u ? \n", i, check[i], normal[test_index * numFeatures + i]);
			flag = 1;
		}
	}
	if(!flag) printf("Results as expected, everything o k \n");
	
	printf("Reached end of test code \n");
	return 0;
}
/*
int main(int argc, char **argv) {
	
	uint32_t numSamples = 2560;
	uint32_t numFeatures = 3;
	uint32_t test_index = 2550;
	int j;
	
	// read command line arguments
	for (j = 1; j < argc; j++) {
		int more = j+1 < argc; // There are more arguments

        if (!strcmp(argv[j],"--nf") && more) { 		  // NUM FEATURES
            numFeatures = (uint32_t) atof(argv[++j]);
		} else if (!strcmp(argv[j],"--ns") && more) { // NUM SAMPLES
			numSamples = (uint32_t) atof(argv[++j]);
		} else if (!strcmp(argv[j],"--ti") && more) { // TEST INDEX
			test_index = (uint32_t) atof(argv[++j]);
		} else if (!strcmp(argv[j],"--d")){			  // DEBUG
			show_debug = 1;
		} else {
            printf("Unknown or not enough arguments for option '%s'.\n\n",
                argv[j]);
            exit(1);
        }
				
	}
	
	// compute / pad number of entries
	uint32_t numEntries = numSamples * numFeatures;
	
	// num samples per 64bits (round down!) (ONE "BLOCK")
	int samples_per_block = 64 / numFeatures;
	
	// if it doesn't align perfectly, have to pad
	if(test_index >= numSamples){
		printf("Test index exceeds number of samples, be reasonable!\n\n");
		exit(1);
	} else if (samples_per_block == 0){
		printf("Maximum 64 features per sample exceeded!\n\n");
		exit(1);
	} else if(samples_per_block * numFeatures != 64){
		printf("Number of features doesn't align w/ 64 bit words, padding.\n");
		
		// how many 64bit blocks do we have?
		int numBlocks = ceil(((float)numSamples) / samples_per_block);
		
		// how many cachelines do we have? (always need all 512 to keep the indexing correct)
		int ceiling = ceil(numBlocks * 64 / 512.0);
		numEntries = ceiling * 512;
	}
	
	// allocate memory
	uint32_t* normal = (uint32_t*) malloc(numSamples * numFeatures * sizeof(uint32_t));
	
	uint32_t* weaved = (uint32_t*) malloc(numEntries * sizeof(uint32_t));
	
	uint32_t* check = (uint32_t*) malloc(numFeatures * sizeof(uint32_t));
	
	int res = 0;
	
	// create and weave example
	create_example_samples(normal, numSamples, numFeatures);
	
	res = weave_samples(weaved, normal, numSamples, numFeatures);
	
	if(res != 0){
		printf("some sort of error in weaving\n\n");
		return 0;
	}
	
	// retrieve 
	retrieve_from_simple_mlweaving(check, weaved, test_index, 32, numFeatures);
	
	printf("numSamples: %u ---- numFeatures: %u ---- numEntries: %u \n" 
	"Testing on sample with index = %u \n"
	"Retrieved == Expected ?\n", numSamples, numFeatures, numEntries, test_index);
	char flag = 0;
	for(int i = 0; i < numFeatures; i++){
		if(check[i] != normal[test_index * numFeatures + i]){
			printf("WARNING:: feature %i: %u == %u ? \n", i, check[i], normal[test_index * numFeatures + i]);
			flag = 1;
		}
	}
	if(!flag) printf("Results as expected, everything o k \n");
	
	printf("Reached end of code \n");
	return 0;
}
*/

