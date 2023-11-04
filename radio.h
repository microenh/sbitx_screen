#pragma once

#include "settings.h"

void init_radio(void);

void select_sub_encoder(SubEncoder item);

void do_agc(void);
void do_mode(void);
void do_record(void);
void do_tx(void);
void do_rit(void);
void do_span(void);
void do_vfo(void);
void do_step(void);
void do_split(void);
void do_band(Band band);

void do_sub_encoder(int change);
void do_main_encoder(int change);