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

void update_vfo_frequency(Vfo vfo, int frequency);
void update_vfo_mode(Vfo vfo, Mode mode);

void update_split(bool on);
void update_record(bool on);
void update_rit(bool on);
void update_tx(bool tx);

// change colors
void update_vfo_state(Vfo vfo, VfoState vfoState);
void update_level_highlight(SubEncoder item, bool on); 
