#pragma once

// pack on single byte boundary
#pragma pack(1)

struct read_bitfield {
    unsigned int: 8;
    unsigned int main_encoder: 16;
    unsigned int e2_encoder: 16;
    unsigned int e1_encoder: 16;

    unsigned int f1_button: 1;
    unsigned int f2_button: 1;
    unsigned int f3_button: 1;
    unsigned int f4_button: 1;
    unsigned int f5_button: 1;
    unsigned int f6_button: 1;
    unsigned int main_button: 1;
    unsigned int e2_button: 1;

    unsigned int e1_button: 1;
    // unsigned int buttons: 9;
};

union read_buffer {
    struct read_bitfield r;
    char d[9];
};


struct write_bitfield {
    unsigned int: 8;        // 1

    unsigned int: 1;
    unsigned int lp: 1;
    unsigned int rx: 1;
    unsigned int tx: 1;
    unsigned int s: 1;
    unsigned int vfo: 1;
    unsigned int nr: 1;
    unsigned int e1: 1;     // 2

    unsigned int att: 1;
    unsigned int smeter_bar_1: 1;
    unsigned int smeter_bar_2: 1;
    unsigned int smeter_bar_3: 1;
    unsigned int smeter_1: 1;
    unsigned int a: 1;
    unsigned int nb: 1;
    unsigned int vol: 1;    // 3

    unsigned int smeter_line: 1;
    unsigned int smeter_9: 1;
    unsigned int smeter_7: 1;
    unsigned int smeter_5: 1;
    unsigned int smeter_3: 1;
    unsigned int b: 1;
    unsigned int an: 1;
    unsigned int rfg: 1;    // 4

    unsigned int main_9l: 4;  // 0b1111 = defg
    unsigned int sql: 1;
    unsigned int: 3;        // 5

    unsigned int main_9r: 3;    // 0bfff = cba
    unsigned int underline_9: 1;
    unsigned int drv: 1;
    unsigned int: 3;        // 6

    unsigned int main_8l: 4;
    unsigned int nr2: 1;
    unsigned int: 3;        // 7

    unsigned int main_8r: 3;
    unsigned int underline_8: 1;
    unsigned int nb2: 1;
    unsigned int: 3;        // 8

    unsigned int main_7l: 4;
    unsigned int an2: 1;
    unsigned int: 3;        // 9

    unsigned int main_7r: 3;
    unsigned int underline_7: 1;
    unsigned int e2: 1;
    unsigned int smeter_10: 1;
    unsigned int: 2;        // 10

    unsigned int main_6l: 4;
    unsigned int dot1: 1;
    unsigned int smeter_plus20: 1;
    unsigned int: 2;        // 11

    unsigned int main_6r: 3;
    unsigned int underline_6: 1;
    unsigned int high: 1;
    unsigned int smeter_20: 1;
    unsigned int: 2;        // 12

    unsigned int main_5l: 4;
    unsigned int low: 1;
    unsigned int: 3;        // 13

    unsigned int main_5r: 3;
    unsigned int underline_5: 1;
    unsigned int shift: 1;
    unsigned int: 3;        // 14

    unsigned int main_4l: 4;
    unsigned int rit: 1;
    unsigned int: 3;        // 15

    unsigned int main_4r: 3;
    unsigned int underline_4: 1;
    unsigned int xit: 1;
    unsigned int: 3;        // 16

    unsigned int main_3l: 4;
    unsigned int dot2: 1;
    unsigned int smeter_plus40: 1;
    unsigned int: 2;        // 17

    unsigned int main_3r: 3;
    unsigned int underline_3: 1;
    unsigned int: 1;
    unsigned int smeter_40: 1;
    unsigned int: 2;        // 18

    unsigned int main_2l: 4;
    unsigned int: 4;        // 19

    unsigned int main_2r: 3;
    unsigned int underline_2: 1;
    unsigned int: 1;
    unsigned int smeter_plus60: 1;
    unsigned int: 2;        // 20

    unsigned int main_1l: 4;
    unsigned int err: 1;
    unsigned int smeter_60: 1;
    unsigned int: 2;        // 21

    unsigned int main_1r: 3;
    unsigned int underline_1: 1;
    unsigned int w_fm: 1;
    unsigned int w: 1;
    unsigned int: 2;        // 22

    unsigned int cw_plus: 1;
    unsigned int cw_minus: 1;
    unsigned int dig_plus: 1;
    unsigned int dig_minus: 1;
    unsigned int dsb: 1;
    unsigned int fm: 1;
    unsigned int usb: 1;
    unsigned int sam: 1;        // 23

    unsigned int drm: 1;
    unsigned int dig: 1;
    unsigned int stereo: 1;
    unsigned int: 1;
    unsigned int dbm: 1;
    unsigned int cw: 1;
    unsigned int lsb: 1;
    unsigned int am: 1;     // 24

    unsigned int hz: 1;
    unsigned int: 3;
    unsigned int smeter_1r: 4;  // 25  // 0b1111 = abcd

    unsigned int: 5;
    unsigned int smeter_1l: 3;  // 26  // 0b111 = fge

    unsigned int k: 1;
    unsigned int: 3;
    unsigned int smeter_2r: 4;  // 27

    unsigned int: 5;
    unsigned int smeter_2l: 3;  // 28

    unsigned int mw_w: 1;
    unsigned int: 3;
    unsigned int smeter_3r: 4;  // 29

    unsigned int mw_m: 1;
    unsigned int: 3;
    unsigned int smeter_db_minus: 1;
    unsigned int smeter_3l: 3;  // 30

    unsigned int: 4;
    unsigned int smeter_bar15: 1;
    unsigned int smeter_bar14: 1;
    unsigned int smeter_bar13: 1;
    unsigned int smeter_bar12: 1;   // 31

    unsigned int pre_1: 1;
    unsigned int pre_2: 1;
    unsigned int: 2;
    unsigned int smeter_bar8: 1;
    unsigned int smeter_bar9: 1;
    unsigned int smeter_bar10: 1;
    unsigned int smeter_bar11: 1;   // 32

    unsigned int att_2: 1;
    unsigned int pre: 1;
    unsigned int att_1: 1;
    unsigned int: 1;
    unsigned int smeter_bar7: 1;
    unsigned int smeter_bar6: 1;
    unsigned int smeter_bar5: 1;
    unsigned int smeter_bar4: 1;    // 33

    unsigned usb_led: 1;
    unsigned lock_led: 1;
    unsigned click: 1;  // not documented, makes a click when the value is changed
    unsigned int: 5;       // 34

    unsigned int rgb_red: 8;    // 35
    unsigned int rgb_green: 8;  // 36
    unsigned int rgb_blue: 8;   // 37
    unsigned int contrast: 8;   // 38
    unsigned int refresh: 8;    // 39

    unsigned int encoder_speed1: 8;     // 40
    unsigned int encoder_speed2: 8;     // 41
    unsigned int encoder_speed3: 8;     // 42
    unsigned int encoder_trans12: 8;    // 43   // 32 steps / revolution
    unsigned int encoder_trans23: 8;    // 44

    unsigned int trans_time: 8;        // 45
};

union write_buffer {
    struct write_bitfield w;
    char d[45];
};

