#pragma once

#include <complex.h>
#include <fftw3.h>

/*
Overview:
The SDR's core is quite simple:
We convert the entire sampled band into frequency domain (the stuff you see in the
waterfalls) by simply passing the I and Q samples to to a library called the FFT3W.

As the FFT turns time samples signals into their frequency bins, it the signals
are spread on both sides of the 0 hz line. 

The FFT lines up not just amplitudes of signals at each frequency but also their phase.
A reverse FFT will add up all these sinewaves and reproduce the signal.

To bring an signal to baseband,we just rotate the bins around until the carrier of 
the SSB signal is at 0 bin. Then, we filter out the rest.

The filter is very simple, we just draw the filter shape in frequency domain
and multiply the FFT bin with that.
 
Once, we are left just the baseband signal in the FFT, we do an inverse FFT
and convert the signals back to time domain.

The basic receiver is just that.

HOW THE FILTER WORKS
This is a little tricky, it took me some time to understand.

To begin with understand that you can convert a series of time samples of a signal
to frequency domain and convert it back to get back the original signal.

Time and Frequency domains are just two ways to represent a signal. We hams are
find it more convenient to handle frequency domain.

We tune to a band, we schedule calls on some, we scan, etc. We all understand 
the waterfall.

So here goes:

1. When you see a single spike on the waterfall, it means there is a cw signal.
When you see the time samples, you will see a continuous sinewave. 

2. Similarly, if there is a blip in the time samples, it spreads across the entire
waterfall.

If you stare at the two above statements, you will realize that what appears as a 
singe sample standing out in one domain corresponds to a a continuous response
in the other.

So, if we convert a signal to frequeny domain, zero all the bins except the
the frequency that we intend to hear, convert it back to time domain and
play it through the speaker? It will work, almost. But not really. The reason is
that some signals fall between two bins. These and other signal types will generate
all kinds of artifacts and clicks. You need a filter that is smooth.
There are ways to smoothen it. 

I am following a method that Phil Karn suggested to me. I first produce a 
'perfect filter' in frequency domain by setting frequency bins of the frequency
that I need to '1' and the rest to zeros. Then, I convert this back to time domain using 
inverse FFT. Now, if you think about the point 1 and 2 made above, you will
can guess that the time domain representation of the filter's shape will
have continuos waveform. 

All the incoming samples are converted to frequency domain in sound_process(). 
The fft_out stores these as frequency bins.
These are fft_out bins are also used to paint the spectrum and waterfall.

You can have any number of radios working with different slices of the spectrum.
At the moment, only ssb (and CW as a single sideband signal) are demodulated.
Each receiver is inserted a node in a linked list that starts at rx_list.

Each receiver is defined by the struct rx.
Each receiver copies the fft_bins to by shifting it around to bring the desired 
to baseband. 

Each tx is also based on a struct rx but it is used to describe and hold state
for the transmission. The data required is the same!
*/

extern const int RX_SAMPLE_RATE;

enum {
	MAX_BINS = 2048
};




struct rx {
	long tuned_bin;	// tuned bin (this should translate to freq) 
	short mode;		// USB/LSB/AM/FM (cw is narrow SSB, so not listed)
					// FFT plan to convert back to time domain
	int low_hz; 
	int high_hz;
	fftwf_plan plan_rev;
	fftwf_complex *fft_freq;
    float *fft_time;
 
	// agc() is called once for every block of samples. The samples
	// are in time-domain. Consider each call to agc as a 'tick'.
	// agc_speed is the max ticks that the agc will hang for
	// agc_loop tracks how many more ticks to go before the decay
	// agc_decay rate sets the slope for agc decay.
	
	int agc_speed;
	int agc_threshold;
	int agc_loop;
	double signal_strength;
	double agc_gain;
	int agc_decay_rate;
	double signal_avg;

	struct filter *filter;	// convolution filter
	int output;				// -1 = nowhere, 0 = audio, rest is a tcp socket
	struct rx* next;
};

// fft_filter.c
struct filter {
	fftwf_complex *fir_coeff;
	int N;
	int L;
	int M;
    fftwf_plan fwd;
    fftwf_plan rev;
};

struct filter *filter_new(int input_length, int impulse_length);
int make_hann_window(float *window, int max_count);
int filter_tune(struct filter *f, float const low, float const high,
                float const kaiser_beta);
void filter_print(struct filter *f);

// vfo.c
struct vfo {
	int freq_hz;
	int phase;
	int phase_increment;
};

void vfo_init_phase_table();
void vfo_start(struct vfo *v, int frequency_hz, int start_phase);
int vfo_read(struct vfo *v);

// fft.c
void fft_init();

// sbitc_sdr.c
struct rx {
	long tuned_bin;	// tuned bin (this should translate to freq) 
	short mode;		// USB/LSB/AM/FM (cw is narrow SSB, so not listed)
					// FFT plan to convert back to time domain
	int low_hz; 
	int high_hz;
	fftwf_plan plan_rev;
	fftwf_complex *fft_freq;
    float *fft_time;
 
	// agc() is called once for every block of samples. The samples
	// are in time-domain. Consider each call to agc as a 'tick'.
	// agc_speed is the max ticks that the agc will hang for
	// agc_loop tracks how many more ticks to go before the decay
	// agc_decay rate sets the slope for agc decay.
	
	int agc_speed;
	int agc_threshold;
	int agc_loop;
	double signal_strength;
	double agc_gain;
	int agc_decay_rate;
	double signal_avg;

	struct filter *filter;	// convolution filter
	int output;				// -1 = nowhere, 0 = audio, rest is a tcp socket
	struct rx* next;
};

extern struct rx *rx_list;