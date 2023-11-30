#include <gtk/gtk.h>
#include <stdbool.h>
// #include <wiringPi.h>

#include "debug.h"
#include "hardware.h"
#include "queue.h"
#include "radio_state.h"
#include "rotary.h"
#include "sdr.h"
#include "si5351.h"
#include "sound.h"
#include "wiringPi.h"

#define LO_USB 40035000
#define LO_LSB 39987000


const int BFO_FREQ = LO_USB;


const int BFO_OFFSET = 24000;
const int TUNING_SHIFT = 0;

const int LPF_A = 5;
const int LPF_B = 6;
const int LPF_C = 10;
const int LPF_D = 11;

struct Queue qremote;

static GString *audio_card;

void hw_set_tx(bool tx) {}

static void hw_filters_off(void) {
    digitalWrite(LPF_A, LOW);
    digitalWrite(LPF_B, LOW);
    digitalWrite(LPF_C, LOW);
    digitalWrite(LPF_D, LOW);
}

static void hw_set_filter_pin(int pin) {
    static int prev_pin = -1;
    if (pin != prev_pin) {
        if (prev_pin > -1)
            digitalWrite(prev_pin, LOW);
        digitalWrite(pin, HIGH);
    }
    prev_pin = pin;
}

void hw_set_filter(gchar filter) {
    int pin = -1;
    switch(filter) {
        case 'A':
            pin = LPF_A;
            break;
        case 'B':
            pin = LPF_B;
            break;
        case 'C':
            pin = LPF_C;
            break;
        case 'D':
            pin = LPF_D;
            break;
    }
    if (pin > -1)
        hw_set_filter_pin(pin); 

}


void set_lpf_40mhz(int frequency){
	int lpf;

	if (frequency < 5500000) {
		lpf = LPF_D;
    } else if (frequency < 10500000) {		
		lpf = LPF_C;
    } else if (frequency < 18500000) {		
		lpf = LPF_B;
    } else { // if (frequency < 30000000) 
		lpf = LPF_A; 
    }
    hw_set_filter_pin(lpf);
}



void hw_set_frequency(int frequency) {
    static int prev_freq = -1;
    if (frequency == prev_freq)
        return;

    int freq_adj;
    switch (get_mode()) {
        case m_lsb:
            freq_adj = 0;
            break;
        case m_usb:
            freq_adj = 0;
            break;
        case m_cw:
            freq_adj = -get_rx_pitch();
            break;
        case m_cwr:
            freq_adj = get_rx_pitch();
            break;
        case m_datar:
            freq_adj = 0;
            break;
        case m_data:
            freq_adj = 0;
            break;
    }
    int adj_frequency = frequency + freq_adj + BFO_FREQ - BFO_OFFSET + TUNING_SHIFT;

    // debug_printf("adj_frequency: %d", adj_frequency);


    si5351bx_setfreq(2, adj_frequency);
    set_lpf_40mhz(frequency);
    prev_freq = frequency;
}

void setup_audio_codec(){
	//configure all the channels of the mixer
	// sound_mixer(audio_card->str, "Input Mux", 0);
	// sound_mixer(audio_card->str, "Line", 1);
	// sound_mixer(audio_card->str, "Mic", 0);
	// sound_mixer(audio_card->str, "Mic Boost", 0);
	// sound_mixer(audio_card->str, "Playback Deemphasis", 0);
 
	// sound_mixer(audio_card->str, "Master", 10);
	// sound_mixer(audio_card->str, "Output Mixer HiFi", 1);
	// sound_mixer(audio_card->str, "Output Mixer Mic Sidetone", 0);
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

void hw_set_af(int level) {
    // sound_mixer(audio_card->str, "Master", level);
}

void hw_set_if(int level) {
    // sound_mixer(audio_card->str, "Capture", level);    
}

static void init_gpio_pins(void) {
    // this requires wiringpi 2.61 (unoffical mods)
	wiringPiSetup();

    const int8_t PINS_IN[] = {7, 0, 2, 3, 12, 13, 14, 21, -1};
    const int8_t PINS_OUT[] = {4, 5, 6, 10, 11, 27, -1};

    // const int8_t PINS_IN[] = {ENC1_A, ENC1_B, ENC2_A, ENC2_B, -1};
    // const int8_t PINS_OUT[] = {-1};

    int8_t *i = (int8_t *) PINS_IN;
    while (*i >= 0) {
        pinMode(*i, INPUT);
        pullUpDnControl(*i, PUD_UP);
        i++;
    }

    i = (uint8_t *) PINS_OUT;
    while (*i >= 0) {
        pinMode(*i, OUTPUT);
        digitalWrite(*i, LOW);
        i++;
    }
	wiringPiISR(ENC1_A, INT_EDGE_BOTH, level_isr);
	wiringPiISR(ENC2_A, INT_EDGE_BOTH, tuning_isr);
}


void hw_init(void) {
    init_gpio_pins();
    hw_filters_off();
    audio_card = g_string_new("hw:0");
    fft_init();
    q_init(&qremote, 8000);

    add_rx(7000000, m_lsb, -3000, -300);
    rx_list->tuned_bin = 512;

    si5351bx_init();
    setup_oscillators();
    setup_audio_codec();
    // sound_thread_start("plughw:0,0");
}

void hw_close(void) {
    hw_set_tx(false);
    for (int i=0; i<3; i++)
        si5351a_clkoff(i);
    hw_filters_off();
    g_string_free(audio_card, true);
}

void hw_set_mode() {
	Mode mode = get_mode();
    int new_bfo = (mode == m_lsb || mode == m_cwr || mode == m_datar) ? LO_LSB : LO_USB;
//    debug_printf("LO: %d", new_bfo);
    si5351bx_setfreq(1, new_bfo);
}