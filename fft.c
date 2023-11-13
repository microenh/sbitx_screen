#include <memory.h>
#include <stdlib.h>

#include "sdr.h"

static fftwf_complex *fft_m;     // holds previous samples for overlap and discard convolution 
static float *fft_m_r;         // holds previous samples for overlap and discard convolution 
static fftwf_complex *fft_in;	// holds the incoming samples in time domain (for rx as well as tx) 
static float *fft_in_r;
static fftwf_complex *fft_out;	// holds the incoming samples in freq domain (for rx as well as tx)
fftwf_complex *fft_spectrum;
fftwf_plan plan_spectrum_r, plan_fwd, plan_fwd_r;
float spectrum_window[MAX_BINS];


void fft_init() {
	fflush(stdout);

	fft_m = fftwf_alloc_complex(MAX_BINS / 2);
    fft_m_r = fftwf_alloc_real(MAX_BINS / 2);
	fft_in = fftwf_alloc_complex(MAX_BINS);
    fft_in_r = fftwf_alloc_real(MAX_BINS);
	fft_out = fftwf_alloc_complex(MAX_BINS);
	fft_spectrum = fftwf_alloc_complex(MAX_BINS);

	plan_fwd = fftwf_plan_dft_1d(MAX_BINS, fft_in, fft_out, FFTW_FORWARD, FFTW_MEASURE);
	plan_fwd_r = fftwf_plan_dft_r2c_1d(MAX_BINS, fft_in_r, fft_out, FFTW_MEASURE);
	// plan_spectrum = fftwf_plan_dft_1d(MAX_BINS, fft_in, fft_spectrum, FFTW_FORWARD, FFTW_MEASURE);
	plan_spectrum_r = fftwf_plan_dft_r2c_1d(MAX_BINS, fft_in_r, fft_spectrum, FFTW_MEASURE);

	// zero up the previous 'M' bins
	memset(fft_m, 0, sizeof(fftwf_complex) * MAX_BINS / 2);
	memset(fft_m_r, 0, sizeof(float) * MAX_BINS / 2);

	make_hann_window(spectrum_window, MAX_BINS);
}

static void fft_reset_m_bins(void) {
	// zero up the previous 'M' bins
	memset(fft_m, 0, sizeof(fftwf_complex) * MAX_BINS / 2);
	memset(fft_m_r, 0, sizeof(float) * MAX_BINS / 2);

	memset(fft_in, 0, sizeof(fftwf_complex) * MAX_BINS);
	memset(fft_out, 0, sizeof(fftwf_complex) * MAX_BINS);
	memset(fft_spectrum, 0, sizeof(fftwf_complex) * MAX_BINS);
	// memset(tx_list->fft_time, 0, sizeof(fftw_complex) * MAX_BINS);
	// memset(tx_list->fft_freq, 0, sizeof(fftwf_complex) * MAX_BINS);
}

static int mag2db(double mag){
	int m = abs(mag) * 10000000;
	
	int c = 31;
	int p = 0x80000000;
	while(c > 0){
		if (p & m)
			break;
		c--;
		p >>= 1;
	}
	return c;
}
