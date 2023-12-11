#pragma once

#include <stdbool.h>

bool init_tmate2(void);

void set_main_move(void (*callback)(int));
void set_e1_move(void (*cb)(int));
void set_e2_move(void (*cb)(int));

void set_main_tap(void (*cb)(void));
void set_e1_tap(void (*cb)(void));
void set_e2_tap(void (*cb)(void));

void set_f1_tap(void (*cb)(void));
void set_f2_tap(void (*cb)(void));
void set_f3_tap(void (*cb)(void));
void set_f4_tap(void (*cb)(void));
void set_f5_tap(void (*cb)(void));
void set_f6_tap(void (*cb)(void));

void set_main_press(void (*cb)(void));
void set_e1_press(void (*cb)(void));
void set_e2_press(void (*cb)(void));

void set_f1_press(void (*cb)(void));
void set_f2_press(void (*cb)(void));
void set_f3_press(void (*cb)(void));
void set_f4_press(void (*cb)(void));
void set_f5_press(void (*cb)(void));
void set_f6_press(void (*cb)(void));

void close_tmate2(void);
void tmate2_tick();

void update_main_digit(int which, char value);
void update_smeter_digit(int which, char value);
void display_main_number(int value);
void display_smeter_number(int value);
void set_rgb(int r, int g, int b);
void set_contrast(int c);
void set_smeter_graph(bool on);
void set_smeter_s(bool on);
void main_encoder_setup(int speed1, int speed2, int speed3, int trans12, int trans23, int trans_time);
void click(void);
