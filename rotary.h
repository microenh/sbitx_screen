#pragma once

typedef struct _rotary {
    int pin_a_no;
    int pin_b_no;
    int b_a_down;
    int old_pina;
} Rotary;

extern Rotary rotary_sub;
extern Rotary rotary_main;

extern volatile int level_ticks;

int check_rotary(Rotary *rotary);
void init_gpio_pins(void);