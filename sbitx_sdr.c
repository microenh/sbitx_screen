#include <stdlib.h>

#include "sdr.h"

struct rx *rx_list = NULL;

static void add_rx(int frequency, short mode, int bpf_low, int bpf_high){

	// we assume that there are <rate> samples / sec, giving us a 48khz slice
	// the tuning can go up and down only by 22 KHz from the center_freq

	struct rx *r = malloc(sizeof(struct rx));
	r->low_hz = bpf_low;
	r->high_hz = bpf_high;
	r->tuned_bin = 512; 
	r->agc_gain = 0.0;

	//create fft complex arrays to convert the frequency back to time
    r->fft_freq = fftwf_alloc_complex(MAX_BINS);

    r->fft_time = fftwf_alloc_real(MAX_BINS);
    // r->plan_fwd = fftwf_plan_dft_r2c_1d(MAX_BINS, fft_in_r, r->fft_freq, FFTW_MEASURE);

	r->plan_rev = fftwf_plan_dft_c2r_1d(MAX_BINS, r->fft_freq, r->fft_time, FFTW_MEASURE);

	r->output = 0;
	r->next = NULL;
	r->mode = mode;
	
	r->filter = filter_new(1024, 1025);
	filter_tune(r->filter, (1.0 * bpf_low) / RX_SAMPLE_RATE, (1.0 * bpf_high) / RX_SAMPLE_RATE , 5);

	if (abs(bpf_high - bpf_low) < 1000){
		r->agc_speed = 300;
		r->agc_threshold = -60;
		r->agc_loop = 0;
    	r->signal_avg = 0;
	} else {
		r->agc_speed = 300;
		r->agc_threshold = -60;
		r->agc_loop = 0;
    	r->signal_avg = 0;
	}

	// the modems are driven by 12000 samples/sec
	// the queue is for 20 seconds, 5 more than 15 sec needed for the FT8

	r->next = rx_list;
	rx_list = r;
}
