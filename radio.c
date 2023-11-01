#include <stdbool.h>
#include <stdio.h>
#include "radio.h"
#include "display.h"
#include "settings.h"
#include "display.h"


typedef struct _radio {
    Agc agc;
    Mode mode;
    Span span;
    Vfo vfo;
    Step step;
    bool rit;
    bool split;
    bool record;
    bool rx_tx;
    SubEncoder subEncoder;
    int level[se_END];
} Radio;

static Radio radio;

void do_10m(void) {}
void do_12m(void) {}
void do_15m(void) {}
void do_17m(void) {}
void do_20m(void) {}
void do_30m(void) {}
void do_40m(void) {}
void do_80m(void) {}

//                                    af, comp, high,  if,  low, mic, pitch, power, wpm
static const int subEncoderMin[]  = {  0,    0,    0,   0,    0,   0,   300,     0,   5};
static const int subEncoderInit[] = { 50,    0, 3000,  50,  200,  50,   600,     0,  13};
static const int subEncoderMax[]  = {100,  100, 4000, 100, 1000, 100,  1500,   100,  40};
static const int subEncoderStep[] = {  1,    1,   50,   1,   50,   1,    10,     1,   1};

void init_radio(void) {
    update_agc(radio.agc);
    update_mode(radio.mode);
    update_span(radio.span);
    update_vfo(radio.vfo);
    update_step(radio.step);
    update_rit(radio.rit);
    update_split(radio.split);
    update_record(radio.record);
    update_rx_tx(radio.rx_tx);
    enable_highlight(radio.subEncoder, true);
    for (int i=0; i<se_END; i++) {
        radio.level[i] = subEncoderInit[i];
        update_level(i, radio.level[i]);
    }
}

void do_agc(void) {
    radio.agc++;
    if (radio.agc >= a_END) {
        radio.agc = 0;
    }
    update_agc(radio.agc);
}

void do_mode(void) {
    radio.mode++;
    if (radio.mode >= m_END) {
        radio.mode = 0;
    }
    update_mode(radio.mode);
}

void do_span(void) {
    radio.span++;
    if (radio.span >= sp_END) {
        radio.span = 0;
    }
    update_span(radio.span);
}

void do_vfo(void) {
    radio.vfo++;
    if (radio.vfo >= v_END) {
        radio.vfo = 0;
    }
    update_vfo(radio.vfo);
}

void do_step(void) {
    radio.step++;
    if (radio.step >= s_END) {
        radio.step = 0;
    }
    update_step(radio.step);
}

void do_rit(void) {
    radio.rit = !radio.rit;
    update_rit(radio.rit);
}

void do_split(void) {
    radio.split = !radio.split;
    update_split(radio.split);
}

void do_record(void) {
    radio.record = !radio.record;
    update_record(radio.record);
}

void do_rx_tx(void) {
    radio.rx_tx = !radio.rx_tx;
    update_rx_tx(radio.rx_tx);
}


void select_small_encoder(SubEncoder item) {
    if (item != radio.subEncoder) {
        enable_highlight(radio.subEncoder, false);
        radio.subEncoder = item;
        enable_highlight(radio.subEncoder, true);  
    }  
}




void do_sub_encoder(int change) {
    int rse = radio.subEncoder;
    int min = subEncoderMin[rse];
    int max = subEncoderMax[rse];
    int step = subEncoderStep[rse];
    int i = radio.level[radio.subEncoder] + change * step;
    if (rse == se_high) {
        min = radio.level[se_low] + step;
    }
    if (rse == se_low) {
        max = radio.level[se_high] - step;
    }
    if (i < min)
        i = min;
    else if (i > max)
        i = max;    
    if (i != radio.level[rse]) {
        radio.level[rse] = i;
        update_level(rse, i);
    }
}

void do_main_encoder(int change) {

}
