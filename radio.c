#include <stdbool.h>
#include "radio.h"
#include "display.h"


void do_high(void) {enable_high(false);}
void do_low(void) {enable_low(false);}
void do_af(void) {enable_af(false);}
void do_if(void) {enable_if(false);}
void do_pitch(void) {enable_pitch(false);}
void do_wpm(void) {enable_wpm(false);}
void do_comp(void) {enable_comp(false);}
void do_mic(void) {enable_mic(false);}
void do_power(void) {enable_power(false);}

void do_agc(void) {}
void do_mode(void) {}
void do_10m(void) {}
void do_12m(void) {}
void do_split(void) {}
void do_15m(void) {}
void do_17m(void) {}
void do_20m(void) {}
void do_record(void) {}
void do_rx(void) {}
void do_tx(void) {}
void do_30m(void) {}
void do_40m(void) {}
void do_80m(void) {}
void do_rit(void) {}
void do_span(void) {}
void do_vfo(void) {}
void do_step(void) {}