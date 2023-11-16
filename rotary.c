#include <stdint.h>
#include <wiringPi.h>

#include "rotary.h"

Rotary
    rotary_sub = {.pin_a_no = ENC1_A, .pin_b_no = ENC1_B},
    rotary_main = {.pin_a_no = ENC2_A, .pin_b_no = ENC2_B};

int check_rotary(Rotary *rotary) {
    int pin_a = digitalRead(rotary->pin_a_no);
    int pin_b = digitalRead(rotary->pin_b_no);
    if (pin_a != rotary->old_pina) {
        rotary->old_pina = pin_a;
        if (pin_a) {
            if (pin_b != rotary->b_a_down) {
                return (pin_b ? 1 : -1);
            }
        } else {
            rotary->b_a_down = pin_b;
        }
    }
    return 0;
}

volatile int level_ticks;
volatile int tuning_ticks;

void level_isr(void) {
	level_ticks += check_rotary(&rotary_sub);
}

void tuning_isr(void) {
	tuning_ticks += check_rotary(&rotary_main);
}

