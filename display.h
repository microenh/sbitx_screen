#pragma once

#include <stdbool.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>

#include "texts.h"
#include "settings.h"

extern bool update_hb_flag;

extern GtkEntry *ent_command;
extern GtkTextBuffer *tb_console;

void init_display(int argc, char **argv);

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
void update_tx_enable(bool enable);

void update_heartbeat(void);

// change colors
void update_vfo_state(Vfo vfo, VfoState vfoState);
void update_level_highlight(SubEncoder item, bool on); 
