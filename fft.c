#include <memory.h>
#include <stdint.h>
#include <stdlib.h>

#include "debug.h"
#include "radio_state.h"
#include "sdr.h"
#include "settings.h"

const int MUTE_MAX = 6; 
static int mute_count = 50;

static fftwf_complex *fft_m;	// holds previous samples for overlap and discard convolution 
static float *fft_m_r;          // holds previous samples for overlap and discard convolution 
static fftwf_complex *fft_in;	// holds the incoming samples in time domain (for rx as well as tx) 
static float *fft_in_r;
static fftwf_complex *fft_out;	// holds the incoming samples in freq domain (for rx as well as tx)
fftwf_complex *fft_spectrum;
fftwf_plan plan_spectrum_r, plan_fwd, plan_fwd_r;
float spectrum_window[MAX_BINS];

void add_rx(int frequency, short mode, int bpf_low, int bpf_high){

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

static double agc2(struct rx *r){
	int i;
	double signal_strength, agc_gain_should_be;

	// do nothing if agc is off
	if (r->agc_speed == -1){
		for (i=0; i < MAX_BINS/2; i++)
			(r->fft_time[i + (MAX_BINS/2)]) *= 10000000.0;
    	return 10000000.0;
	}

	// find the peak signal amplitude

	signal_strength = 0.0;
	for (i=0; i < MAX_BINS/2; i++){
		float s = r->fft_time[i + (MAX_BINS/2)] * 1000.0;
		if (signal_strength < s) 
			signal_strength = s;
	}

    // static float max_signal_strength = 0.0;
    // if (max_signal_strength < signal_strength) {
    //     max_signal_strength = signal_strength;
    //     printf("max_signal_strength: %8.2f\r\n", max_signal_strength);
    // }

	// also calculate the moving average of the signal strength
	r->signal_avg = (r->signal_avg * 0.93) + (signal_strength * 0.07);
	if (signal_strength == 0.0)
		agc_gain_should_be = 10000000.0;
	else
		agc_gain_should_be = 100000000000.0 / signal_strength;
	r->signal_strength = signal_strength;
	// printf("Agc temp, g:%g, s:%g, f:%g ", r->agc_gain, signal_strength, agc_gain_should_be);

	double agc_ramp = 0.0;

	// climb up the agc quickly if the signal is louder than before 
	if (agc_gain_should_be < r->agc_gain){
		r->agc_gain = agc_gain_should_be;
		// reset the agc to hang count down 
    	r->agc_loop = r->agc_speed;
		// printf("attack %g %d ", r->agc_gain, r->agc_loop);
	} else if (r->agc_loop <= 0){
		agc_ramp = (agc_gain_should_be - r->agc_gain) / (MAX_BINS/2);	
		// printf("release %g %d ",  r->agc_gain, r->agc_loop);
	}
	// else if (r->agc_loop > 0)
	//  	printf("hanging %g %d ", r->agc_gain, r->agc_loop);
 
	if (agc_ramp != 0){
		// printf("Ramping from %g ", r->agc_gain);
  		for (i = 0; i < MAX_BINS/2; i++){
	  		/* __real__ */ (r->fft_time[i + (MAX_BINS/2)]) *= r->agc_gain;
		}
		r->agc_gain += agc_ramp;		
		// printf("by %g to %g ", agc_ramp, r->agc_gain);
	} else 
  		for (i = 0; i < MAX_BINS/2; i++)
	  		/* __real__ */ (r->fft_time[i + (MAX_BINS/2)]) *= r->agc_gain;

	// printf("\n");
	r->agc_loop--;

	// printf("%d:s meter: %d %d %d \n", count++, (int)r->agc_gain, (int)r->signal_strength, r->agc_loop);
	return 100000000000.0 / r->agc_gain;  
}

void rx_process(
	int32_t *input_rx, int32_t *input_mic, 
	int32_t *output_speaker, int32_t *output_tx, 
	int n_samples) {

	//STEP 1: first add the previous M samples to
    memcpy(fft_in_r, fft_m_r, (MAX_BINS/2) * sizeof(float));

	// for (int i=0; i<MAX_BINS/2; i++)
    //     fft_in_r[i] = fft_m_r[i];

	//STEP 2: then add the new set of samples
	// j is the index into incoming samples, starting at zero
	// i is the index into the time samples, picking from 
	// the samples added in the previous step
	// gather the samples into a time domain array 
	for (int i=MAX_BINS/2, j=0; i < MAX_BINS; i++, j++) {
        fft_in_r[i] = fft_m_r[j] = input_rx[j] * 5e-09;
	}

	//STEP 3: convert the time domain samples to  frequency domain
	struct rx *r = rx_list;
	fftwf_execute(plan_fwd_r);
	// fftwf_execute(r->plan_fwd);

	//STEP 3B: this is a side line, we use these frequency domain
	// values to paint the spectrum in the user interface
	// I discovered that the raw time samples give horrible spectrum
	// and they need to be multiplied with a window function 
	// they use a separate fft plan
	// NOTE: the spectrum update has nothing to do with the actual
	// signal processing. If you are not showing the spectrum or the
	// waterfall, you can skip these steps
	for (int i=0; i<MAX_BINS; i++)
	    fft_in_r[i] *= spectrum_window[i];


	fftwf_execute(plan_spectrum_r);

	// the spectrum display is updated - not needed, called elsewhere
	// spectrum_update();

	// ... back to the actual processing, after spectrum update  

	// we may add another sub receiver within the pass band later,
	// hence, the linked list of receivers here
	// at present, we handle just the first receiver
	// struct rx *r = rx_list;
	
	//STEP 4: we rotate the bins around by r->tuned_bin and apply filter

    // needed because reverse c2r DFT changes input (r->fft_freq)
    memset(r->fft_freq, 0, sizeof(fftwf_complex) * MAX_BINS);

    for (int i=MAX_BINS-r->tuned_bin, b=0; i<MAX_BINS; i++, b++)
        r->fft_freq[i] = fft_out[b] * r->filter->fir_coeff[i];

    for (int i=0, b=r->tuned_bin; b<MAX_BINS/2; i++, b++)
        r->fft_freq[i] = fft_out[b] * r->filter->fir_coeff[i];    

    // for (int i=1, j=MAX_BINS-i; i<MAX_BINS/2; i++, j--) {
    //     __real__ r->fft_freq[i] += __real__ r->fft_freq[j];
    //     __imag__ r->fft_freq[i] -= __imag__ r->fft_freq[j];
    // }


    // STEP 5: zero out the other sideband - not needed, handled by filter
    // STEP 6: apply the filter - not needed, done in rotate step

	//STEP 7: convert back to time domain	
    fftwf_execute(r->plan_rev);

	//STEP 8 : AGC
	agc2(r);
	
	//STEP 9: send the output back to where it needs to go
	int is_digital = 0;
	int fft_max = 0;
	int fft_min = 0x7fffffff;
	if (rx_list->output == 0){
		for (int i=0, j=MAX_BINS/2; j<MAX_BINS; i++, j++){
			output_speaker[i] = (int) r->fft_time[j];
			output_tx[i] = 0;
		if (output_speaker[i] > fft_max)
			fft_max = output_speaker[i];
		if (output_speaker[i] < fft_min)
			fft_min = output_speaker[i];
		}

		//push the samples to the remote audio queue, decimated to 16000 samples/sec
		//for (int i=0; i<MAX_BINS/2; i+=6)
		//	q_write(&qremote, output_speaker[i]);

	}

	if (mute_count){
		memset(output_speaker, 0, MAX_BINS/2 * sizeof(int32_t));
		mute_count--;
	}

	// push the data to any potential modem 
	// modem_rx(rx_list->mode, output_speaker, MAX_BINS/2);
}

void set_rx_filter() {
	Mode mode = get_mode();
	if (mode == m_lsb || mode == m_cwr) {
        // puts("LSB");
		filter_tune(rx_list->filter, 
			(1.0 * -get_high()) / RX_SAMPLE_RATE, 
			(1.0 * -get_low()) / RX_SAMPLE_RATE, 
			5);
    } else {
        // puts("USB");
		filter_tune(rx_list->filter, 
			(1.0 * get_low()) / RX_SAMPLE_RATE, 
			(1.0 * get_high()) / RX_SAMPLE_RATE, 
			5);
    }
    #if 0
    FILE *out;
    out = fopen("/home/pi/data.txt", "w");
    complex float *fir = rx_list->filter->fir_coeff;
    for (int i = 0; i < MAX_BINS; i++, fir++) {
        // if (cabs(*fir) > 0.01)
            // fprintf(out, "%4d: %5.2f + %5.2fi\r\n", i, creal(*fir), cimag(*fir));
            fprintf(out, "%d, %f, %f\r\n", i, __real__ *fir, __imag__ *fir);
    }
    fclose(out);
    printf("Saved filter to file.\r\n");
    #endif
}