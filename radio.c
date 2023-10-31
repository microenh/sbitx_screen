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
} Radio;

static Radio radio;

void do_high(void) {enable_high(false);}
void do_low(void) {enable_low(false);}
void do_af(void) {enable_af(false);}
void do_if(void) {enable_if(false);}
void do_pitch(void) {enable_pitch(false);}
void do_wpm(void) {enable_wpm(false);}
void do_comp(void) {enable_comp(false);}
void do_mic(void) {enable_mic(false);}
void do_power(void) {enable_power(false);}

void do_10m(void) {}
void do_12m(void) {}
void do_15m(void) {}
void do_17m(void) {}
void do_20m(void) {}
void do_30m(void) {}
void do_40m(void) {}
void do_80m(void) {}

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
