#pragma once

#define ENC1_A  (13)
#define ENC1_B  (12)
#define ENC1_SW (14)

#define ENC2_A  (2)
#define ENC2_B  (0)
#define ENC2_SW (3)

typedef struct _rotary {
    int pin_a_no;
    int pin_b_no;
    int b_a_down;
    int old_pina;
} Rotary;

extern Rotary rotary_sub;
extern Rotary rotary_main;

extern volatile int level_ticks;
extern volatile int tuning_ticks;

int check_rotary(Rotary *rotary);
void level_isr(void);
void tuning_isr(void);