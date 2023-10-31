#pragma once

#include <stdbool.h>
#include "texts.h"
#include "settings.h"

void init_display(int argc, char **argv);

void update_console(char *text);
void update_date(char *text);
void update_level(SubEncoder item, int value);

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

void enable_highlight(SubEncoder item, bool on); 