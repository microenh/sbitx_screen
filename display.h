#pragma once

#include <stdbool.h>

void init_display(int argc, char **argv);

void update_console(char *text);
void update_step(char *text);
void update_vfo(char *text);
void update_span(char *text);
void update_rit(char *text);
void update_vfob(char *text);
void update_vfoa(char *text);
void update_date(char *text);
void update_power(char *text);
void update_record(char *text);
void update_mic(char *text);
void update_comp(char *text);
void update_wpm(char *text);
void update_pitch(char *text);
void update_split(char *text);
void update_mode(char *text);
void update_low(char *text);
void update_high(char *text);
void update_agc(char *text);
void update_if(char *text);
void update_af(char *text);

void enable_high(bool enable);
void enable_low(bool enable);
void enable_af(bool enable);
void enable_if(bool enable);
void enable_pitch(bool enable);
void enable_wpm(bool enable);
void enable_comp(bool enable);
void enable_mic(bool enable);
void enable_power(bool enable);