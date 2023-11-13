#pragma once

#include <complex.h>
#include <fftw3.h>

// the filter definitions
struct filter {
	fftwf_complex *fir_coeff;
	int N;
	int L;
	int M;
    fftwf_plan fwd;
    fftwf_plan rev;
};

// fft_filter.c
struct filter *filter_new(int input_length, int impulse_length);
int filter_tune(struct filter *f, float const low, float const high,
                float const kaiser_beta);
void filter_print(struct filter *f);