#pragma once

#include <stdbool.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>

#include "settings.h"

#define BAND_STACK_SIZE 3

typedef struct _vfoData {
    Agc agc;
    Mode mode;
    int frequency;
    int level[se_END];
} VfoData;

typedef struct _miscSettings {
    bool rit;
    int rit_value;
    bool split;
    Span span;
    Step step;
} MiscSettings;

typedef struct _bandStackEntry {
    VfoData vfoData[v_END];
    MiscSettings miscSettings;
} BandStackEntry;

typedef struct _bandStack {
    int current;
    BandStackEntry bandStackEntry[BAND_STACK_SIZE];
} BandStack;

typedef struct _radio {
    Vfo vfo;
    SubEncoder subEncoder;
    VfoData vfoData[v_END];
    MiscSettings miscSettings;
    bool tx;
    bool record;
    bool tx_lock;
    gchar callsign[15];
    gchar grid[10];
    BandStack bandStack[b_END];   
} Radio;

void init_radio(void);
void initial_radio_settings(Radio *radio);

void select_sub_encoder(SubEncoder item);

void do_agc(void);
void do_mode_inc(void);
void do_mode(Mode m);
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
void do_frequency(int frequency);

extern char prefix[];

void save_settings(void);

const gchar * const get_callsign(void);
void set_callsign(const gchar * const callsign);

const gchar * const get_grid(void);
void set_grid(const gchar * const grid);

const bool get_tx_lock(void);
void set_tx_lock(const bool tx_lock);