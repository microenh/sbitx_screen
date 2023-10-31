#include <stdint.h>
#include <wiringPi.h>

#include "rotary.h"

#define ENC1_A  (13)
#define ENC1_B  (12)
#define ENC1_SW (14)

#define ENC2_A  (0)
#define ENC2_B  (2)
#define ENC2_SW (3)

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
                return (pin_b ? -1 : 1);
            }
        } else {
            rotary->b_a_down = pin_b;
        }
    }
    return 0;
}

volatile int level_ticks;

static void level_isr(void){
	int level = check_rotary(&rotary_sub);
	if (level < 0)
		level_ticks++;
	else if (level > 0)
		level_ticks--;	
}


void init_gpio_pins(void) {
    // this requires wiringpi 2.61 (unoffical mods)
	wiringPiSetup();

    // const int8_t PINS_IN[] = {7, 0, 2, 3, 12, 13, 14, 21, -1};
    // const int8_t PINS_OUT[] = {4, 5, 6, 10, 11, 27, -1};

    const int8_t PINS_IN[] = {ENC1_A, ENC1_B, ENC2_A, ENC2_B, -1};
    const int8_t PINS_OUT[] = {-1};

    int8_t *i = (int8_t *) PINS_IN;
    while (*i >= 0) {
        pinMode(*i, INPUT);
        pullUpDnControl(*i, PUD_UP);
        i++;
    }

    i = (uint8_t *) PINS_OUT;
    while (*i >= 0) {
        pinMode(*i, OUTPUT);
        digitalWrite(*i, LOW);
        i++;
    }
	wiringPiISR(ENC1_A, INT_EDGE_BOTH, level_isr);
	wiringPiISR(ENC1_B, INT_EDGE_BOTH, level_isr);
}
