#define _GNU_SOURCE 1
#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stddef.h>
#include <stdlib.h>

#include "sdr.h"

// Modified Bessel function of the 0th kind, used by the Kaiser window
const float i0(float const z){
    const float t = (z*z)/4;
    float sum = 1 + t;
    float term = t;
    for(int k=2; k<40; k++){
        term *= t/(k*k);
        sum += term;
        if(term < 1e-12 * sum)
            break;
    }
    return sum;
}

// Modified Bessel function of first kind
const float i1(float const z){
    const float t = (z*z)/4;
    float term = 1;
    float sum = term;

    for(int k=1; k<40; k++){
        term *= t / (k*(k+1));
        sum += term;
        if(term < 1e-12 * sum)
            break;
    }
    return 0.5 * z * sum;
}

// Compute an entire Kaiser window
// More efficient than repeatedly calling kaiser(n,M,beta)
int make_kaiser(float * const window,unsigned int const M,float const beta){
    assert(window != NULL);
    if (window == NULL)
        return -1;
    // Precompute unchanging partial values
    float const numc = M_PI * beta;
    float const inv_denom = 1.0 / i0(numc); // Inverse of denominator
    float const pc = 2.0 / (M-1);

    // The window is symmetrical, so compute only half of it and mirror
    // this won't compute the middle value in an odd-length sequence
    for (int n = 0; n < M/2; n++){
        float const p = pc * n  - 1;
        window[M-1-n] = window[n] = i0(numc * sqrtf(1-p*p)) * inv_denom;
    }
    // If sequence length is odd, middle value is unity
    if (M & 1)
        window[(M-1)/2] = 1; // The -1 is actually unnecessary

  return 0;
}

const static float hann(int const n,int const M) {
    return 0.5 - 0.5 * cos(2*M_PI*n/(M-1));
}

int make_hann_window(float *window, int max_count) {
	// apply to the entire fft (MAX_BINS)
	for (int i = 0; i < max_count; i++)
		window[i] = hann(i, max_count);	 
}

// Apply Kaiser window to filter frequency response
// "response" is SIMD-aligned array of N complex floats
// Impulse response will be limited to first M samples in the time domain
// Phase is adjusted so "time zero" (center of impulse response) is at M/2
// L and M refer to the decimated output
int window_filter(struct filter *f, float const beta){
    fftwf_execute(f->rev);

    float kaiser_window[f->M];
    make_kaiser(kaiser_window, f->M, beta);

    // Round trip through FFT/IFFT scales by N
    float const gain = 1.0;

	//shift the buffer to make it causal, apply window and gain
    for(int n=f->M-1; n>=0; n--)
        f->fir_coeff[n] = f->fir_coeff[(n-f->M/2+f->N) % f->N] * kaiser_window[n] * gain;
	
    // Pad with zeroes on right side
    memset(f->fir_coeff+f->M, 0, (f->N-f->M) * sizeof(*f->fir_coeff));

    // Now back to frequency domain
    fftwf_execute(f->fwd);

    return 0;
}

struct filter *filter_new(int input_length, int impulse_length) {

	struct filter *f = malloc(sizeof(struct filter));
	f->L = input_length;
	f->M = impulse_length;
    f->N = f->L + f->M - 1;
    f->fir_coeff = fftwf_alloc_complex(f->N);

    f->fwd = fftwf_plan_dft_1d(f->N, f->fir_coeff, f->fir_coeff, FFTW_FORWARD, FFTW_MEASURE);
    f->rev = fftwf_plan_dft_1d(f->N, f->fir_coeff, f->fir_coeff, FFTW_BACKWARD, FFTW_MEASURE);

	return f;
}

int filter_tune(struct filter *f, float const low, float const high,
                float const kaiser_beta) {

    if (isnan(low) || isnan(high) || isnan(kaiser_beta))
        return -1;

    float m_low = fabs(low);
    float m_high = fabs(high);

    assert(m_low <= 0.5);
    assert(m_high <= 0.5);

    if (m_low > m_high) {
        float temp = m_low;
        m_low = m_high;
        m_high = temp;
    }

    float gain = 1.0/((float)f->N);
	// printf("# Gain is %lf\n", gain);
	// printf("# filter elements %d\n", f->N);

    for(int n = 0; n < f->N; n++){
        float s = (float)(n) / f->N;

        if (s >= m_low && s <= m_high)
            f->fir_coeff[n] = gain;
        else
            f->fir_coeff[n] = 0;
        // printf("#1 %d  %g  %g %g before windowing: %g,%g\n", n, s, low, high, creal(f->fir_coeff[n]), cimag(f->fir_coeff[n]));
    }

    window_filter(f, kaiser_beta);

    FILE *out;
    out = fopen("filter_data.csv","w");
    fprintf(out, "id,real,imag\r\n");
    for (int i=0; i<f->N; i++)
        fprintf(out, "%d,%f,%f\r\n", i, __real__ f->fir_coeff[i], __imag__ f->fir_coeff[i]);
    fclose(out);

    return 0;
}

void filter_print(struct filter *f){

    printf("#Filter windowed FIR frequency coefficients\n");
	for(int n=0;n<f->N;n++)
        printf("%d,%.17f,%.17f\n", n, crealf(f->fir_coeff[n]), cimagf(f->fir_coeff[n]));
}

/*
int main(int argc, char **argv){
	float window[30];

	struct filter *f = filter_new(1024,1025);

	float low_cutoff = atof(argv[1])/22000.0;
	float high_cutoff = atof(argv[2])/22000.0;
	filter_tune(f, low_cutoff, high_cutoff, 5.0);
}
*/
