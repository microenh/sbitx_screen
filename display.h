#pragma once

#include <stdbool.h>
#include "texts.h"
#include "settings.h"

void init_display(int argc, char **argv);

void update_console(char *text);
void update_date(char *text);

void update_step(Step step);
void update_span(Span span);
void update_mode(Mode mode);
void update_agc(Agc agc);
void update_vfo(Vfo vfo);

void update_vfoa(int frequency, Mode mode);
void update_vfob(int frequency, Mode mode);

void update_split(bool on);
void update_record(bool on);
void update_rit(bool on);
void update_rx_tx(bool rx_tx);

void update_power(int level);
void update_mic(int level);
void update_comp(int level);
void update_wpm(int wpm);
void update_pitch(int pitch);
void update_low(int frequency);
void update_high(int frequncy);
void update_if(int level);
void update_af(int level);

void enable_highlight(SmEncoder item, bool on); 