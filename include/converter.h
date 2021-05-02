
#ifndef CONVERTER
#define CONVERTER
#include <stdint.h>
#include <stdlib.h>

uint32_t *weave_samples_wrapper(uint32_t* data,size_t rows,size_t cols);

int create_example_samples(uint32_t *dest, uint32_t numSamples, uint32_t numFeatures);

int weave_samples(uint32_t *dest, uint32_t *src, uint32_t numSamples, uint32_t numFeatures);

void retrieve_from_simple_mlweaving(uint32_t *dest, uint32_t *src, uint32_t index, uint32_t num_bits, uint32_t numFeatures);

int converter_test(uint32_t numSamples,	uint32_t numFeatures, uint32_t test_index);

uint32_t numberOfEntries(uint32_t numSamples,	uint32_t numFeatures);

#endif