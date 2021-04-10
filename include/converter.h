
#ifndef CONVERTER
#define CONVERTER
#include <stdint.h>
#include <stdlib.h>

int create_naive_example_samples(uint32_t *dest, uint32_t numSamples, uint32_t numFeatures);

int create_example_samples(uint32_t *dest, uint32_t numSamples, uint32_t numFeatures);

int weave_samples_simple(uint32_t *dest, uint32_t *src, uint32_t numSamples, uint32_t numFeatures);

void retrieve_from_simple_mlweaving(uint32_t *dest, uint32_t *src, uint32_t index, uint32_t num_bits, uint32_t numFeatures);


// int test();
#endif