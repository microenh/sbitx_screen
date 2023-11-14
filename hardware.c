#include <stdbool.h>

#include "console.h"
#include "hardware.h"
#include "queue.h"
#include "radio_state.h"
#include "sdr.h"
#include "si5351.h"
#include "sound.h"
#include "wiringPi.h"

void hw_set_tx(bool tx) {}

struct Queue qremote;

const int BFO_FREQ = 40035000;
const int BFO_OFFSET = 24000;
const int TUNING_SHIFT = 0;

const int LPF_A = 5;
const int LPF_B = 6;
const int LPF_C = 10;
const int LPF_D = 11;


void set_lpf_40mhz(int frequency){
    static int prev_lpf = -1;
    if (frequency == -1) {
        digitalWrite(LPF_A, LOW);
        digitalWrite(LPF_B, LOW);
        digitalWrite(LPF_C, LOW);
        digitalWrite(LPF_D, LOW);
        prev_lpf = -1;
        return;
    }
	int lpf;

	if (frequency < 5500000)
		lpf = LPF_D;
	else if (frequency < 10500000)		
		lpf = LPF_C;
	else if (frequency < 18500000)		
		lpf = LPF_B;
	else // if (frequency < 30000000)
		lpf = LPF_A; 

	if (lpf == prev_lpf){
		return;
	}

    // GString *text = g_string_new(NULL);
    // g_string_printf(text, "LPF: Off %d, On %d", prev_lpf, lpf);
    // update_console(text->str);
    // g_string_free(text, true);    
    if (prev_lpf > -1)
        digitalWrite(prev_lpf, LOW);
	digitalWrite(lpf, HIGH); 
	prev_lpf = lpf;
}

void hw_set_frequency(int frequency) {
    static int prev_freq = -1;
    if (frequency == prev_freq)
        return;
    // GString *text = g_string_new(NULL);
    // g_string_printf(text, "Freq: %d", frequency);
    // update_console(text->str);
    // g_string_free(text, true);    

    int freq_adj;
    switch (get_mode()) {
        case m_cw:
            freq_adj = -get_rx_pitch();
            break;
        case m_cwr:
            freq_adj = get_rx_pitch();
            break;
        default:
            freq_adj = 0;
            break;
    }
    int adj_frequency = frequency + freq_adj + BFO_FREQ - BFO_OFFSET + TUNING_SHIFT;
    si5351bx_setfreq(2, adj_frequency);
    set_lpf_40mhz(frequency);
    prev_freq = frequency;
}



void setup_audio_codec(){
    GString * const audio_card = g_string_new("hw:0");

	//configure all the channels of the mixer
	sound_mixer(audio_card->str, "Input Mux", 0);
	sound_mixer(audio_card->str, "Line", 1);
	sound_mixer(audio_card->str, "Mic", 0);
	sound_mixer(audio_card->str, "Mic Boost", 0);
	sound_mixer(audio_card->str, "Playback Deemphasis", 0);
 
	sound_mixer(audio_card->str, "Master", 10);
	sound_mixer(audio_card->str, "Output Mixer HiFi", 1);
	sound_mixer(audio_card->str, "Output Mixer Mic Sidetone", 0);

    g_string_free(audio_card, true);
}

void setup_oscillators(){
	// initialize the SI5351

	delay(200);
	si5351bx_init();
	delay(200);
	si5351bx_setfreq(1, BFO_FREQ);

	// delay(200);
	// si5351bx_setfreq(1, bfo_freq);

	// si5351_reset();
}

void hw_init(void) {
    fft_init();
    q_init(&qremote, 8000);

    add_rx(7000000, m_lsb, -3000, -300);
    rx_list->tuned_bin = 512;

    si5351bx_init();
    setup_oscillators();
    setup_audio_codec();
    sound_thread_start("plughw:0,0");
}

void hw_close(void) {
    hw_set_tx(false);
    for (int i=0; i<3; i++)
        si5351a_clkoff(i);
    set_lpf_40mhz(-1);
}

