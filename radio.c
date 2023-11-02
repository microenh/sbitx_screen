#include <stdbool.h>
#include <stdio.h>
#include "radio.h"
#include "display.h"
#include "settings.h"
#include "display.h"

#define BAND_STACK_SIZE 4

typedef struct _bandStackEntry {
    bool valid;
    int frequency;
    Mode mode;
    int low;
    int high;
    int if_setting;
    Agc agc;
    int pitch;
    int mic;
    int comp;
    int power;
    Step step;
    Span span;
} BandStackEntry;

typedef struct _bandStack {
    int current;
    BandStackEntry bandStackEntry[BAND_STACK_SIZE];
} BandStack;

typedef struct _radio {
    Agc agc;
    Mode mode[v_END];
    int frequency[v_END];
    Span span;
    Vfo vfo;
    Step step;
    bool rit;
    int rit_value;
    bool split;
    bool record;
    bool rx_tx;
    SubEncoder subEncoder;
    int level[se_END];
    BandStack bandStack[b_END];   
} Radio;

static Radio radio;

static void update_vfo_states(void);


static Band get_band(void) {
    int f = radio.frequency[radio.vfo];
    if      (f >=  3500000 && f <=  4000000)
        return b_80m;
    else if (f >=  7000000 && f <=  7300000)
        return b_40m;
    else if (f >= 10100000 && f <= 10150000)
        return b_30m;
    else if (f >= 14000000 && f <= 14350000)
        return b_20m;
    else if (f >= 18068000 && f <= 18168000)
        return b_17m;
    else if (f >= 21000000 && f <= 21450000)
        return b_15m;
    else if (f >= 24890000 && f <= 24990000)
        return b_12m;
    else if (f >= 28000000 && f <= 29700000)
        return b_10m;
    return b_END;
}

static void update_display(void) {
    update_agc(radio.agc);
    update_mode(radio.mode[radio.vfo]);
    update_span(radio.span);
    update_vfo(radio.vfo);
    update_step(radio.step);
    update_rit(radio.rit);
    update_split(radio.split);
    update_record(radio.record);
    update_rx_tx(radio.rx_tx);
    update_rx_tx_state(radio.rx_tx);
    enable_highlight(radio.subEncoder, true);
    for (int i=0; i<se_END; i++) {
        update_level(i, radio.level[i]);
    }
    for (int i=0; i<v_END; i++) {
        update_vfo_frequency(i, radio.frequency[i]);
        update_vfo_mode(i, radio.mode[i]);
    }
    update_vfo_states();

}

void do_band(Band band) {
    Band cur_band = get_band();
    if (cur_band != b_END) {
        // save current settings
        int tos = (radio.bandStack[cur_band].current - 1) % BAND_STACK_SIZE;
        radio.bandStack[cur_band].current = tos;
        BandStackEntry bse = {
            .valid = true,
            .frequency = radio.frequency[radio.vfo],
            .mode = radio.mode[radio.vfo],
            .low = radio.level[se_low],
            .high = radio.level[se_high],
            .if_setting = radio.level[se_if],
            .agc = radio.agc,
            .pitch = radio.level[se_pitch],
            .mic = radio.level[se_mic],
            .comp = radio.level[se_comp],
            .power = radio.level[se_power],
            .step = radio.step,
            .span = radio.span
        };
        radio.bandStack[cur_band].bandStackEntry[tos] = bse;        
    }
    BandStackEntry bse = radio.bandStack[band].bandStackEntry[radio.bandStack[band].current];
    if (bse.valid) {
        radio.agc = bse.agc;
        for (int i=0; i<v_END; i++) {
            radio.mode[i] = bse.mode;
            radio.frequency[i] = bse.frequency;
        }
        radio.span = bse.span;
        radio.step = bse.step;
        radio.rit = false;
        radio.rit_value = 0;
        radio.split = false;
        radio.record = false;
        radio.rx_tx = false;
        radio.subEncoder = se_af;
        radio.level[se_comp] = bse.comp;
        radio.level[se_high] = bse.high;
        radio.level[se_if] = bse.if_setting;
        radio.level[se_low] = bse.low;
        radio.level[se_mic] = bse.mic;
        radio.level[se_pitch] = bse.pitch;
        radio.level[se_power] = bse.power;
        update_display();
    }
}



//                                    af, comp, high,  if,  low, mic, pitch, power, wpm
static const int subEncoderMin[]  = {  0,    0,    0,   0,    0,   0,   300,     0,   5};
static const int subEncoderInit[] = { 50,    0, 3000,  50,  200,  50,   600,     0,  13};
static const int subEncoderMax[]  = {100,  100, 4000, 100, 1000, 100,  1500,   100,  40};
static const int subEncoderStep[] = {  1,    1,   50,   1,   50,   1,    10,     1,   1};


void init_radio(void) {
    for (int i=0; i<se_END; i++) {
        radio.level[i] = subEncoderInit[i];
    }
    for (int i=0; i<v_END; i++) {
        radio.frequency[i] = 7000000;
        radio.mode[i] = m_lsb;
    }

    update_display();
}

static void update_vfo_states(void) {
    Vfo sel, unsel;
    VfoState selState, unselState;
    sel = radio.vfo;
    unsel = (radio.vfo + 1) % v_END;
    if (radio.split) {
        selState = radio.rx_tx ? vs_rx_inactive : vs_rx_active;
        unselState = radio.rx_tx ? vs_tx_active : vs_tx_inactive;
    } else {
        selState = radio.rx_tx ? vs_tx_active : vs_rx_active;
        unselState = vs_inactive;
    }
    update_vfo_state(sel, selState);
    update_vfo_state(unsel, unselState);
}

void do_agc(void) {
    radio.agc++;
    if (radio.agc >= a_END) {
        radio.agc = 0;
    }
    update_agc(radio.agc);
}

void do_mode(void) {
    if (radio.rx_tx) return;
    Mode new_mode = radio.mode[radio.vfo] + 1;
    if (new_mode >= m_END)
        new_mode = 0;
    radio.mode[radio.vfo] = new_mode;
    update_mode(new_mode);
    update_vfo_mode(radio.vfo, new_mode);
}

void do_span(void) {
    radio.span++;
    if (radio.span >= sp_END) {
        radio.span = 0;
    }
    update_span(radio.span);
}

void do_vfo(void) {
    if (radio.rx_tx) return;
    radio.vfo++;
    if (radio.vfo >= v_END) {
        radio.vfo = 0;
    }
    update_vfo(radio.vfo);
    update_vfo_states();
}

void do_step(void) {
    radio.step++;
    if (radio.step >= s_END) {
        radio.step = 0;
    }
    update_step(radio.step);
    int adj = radio.frequency[radio.vfo] % step_values[radio.step];
    if (adj) {
        radio.frequency[radio.vfo] -= adj;
        update_vfo_frequency(radio.vfo, radio.frequency[radio.vfo]);
    }
}

static int adj_frequency(Vfo vfo){
    int frequency = radio.frequency[vfo];
    if (radio.rit)
        frequency += radio.rit_value;
    return frequency;
}

void do_rit(void) {
    if (radio.rx_tx) return;
    radio.rit = !radio.rit;
    update_rit(radio.rit);
    if (radio.rit_value) {
        for (int i=0; i<v_END; i++)
            if (!radio.split || i == radio.vfo)
                update_vfo_frequency(i, adj_frequency(i));        
    }
}

void do_split(void) {
    if (radio.rx_tx) return;
    radio.split = !radio.split;
    update_split(radio.split);
    update_vfo_states();
}

void do_record(void) {
    radio.record = !radio.record;
    update_record(radio.record);
}

void do_rx_tx(void) {
    radio.rx_tx = !radio.rx_tx;
    update_rx_tx(radio.rx_tx);
    update_vfo_states();
    update_rx_tx_state(radio.rx_tx);
}


void select_small_encoder(SubEncoder item) {
    if (item != radio.subEncoder) {
        enable_highlight(radio.subEncoder, false);
        radio.subEncoder = item;
        enable_highlight(radio.subEncoder, true);  
    }  
}

void do_sub_encoder(int change) {
    int rse = radio.subEncoder;
    int min = subEncoderMin[rse];
    int max = subEncoderMax[rse];
    int step = subEncoderStep[rse];
    int i = radio.level[radio.subEncoder] + change * step;
    if (rse == se_high) {
        min = radio.level[se_low] + step;
    }
    if (rse == se_low) {
        max = radio.level[se_high] - step;
    }
    if (i < min)
        i = min;
    else if (i > max)
        i = max;    
    if (i != radio.level[rse]) {
        radio.level[rse] = i;
        update_level(rse, i);
    }
}


void do_main_encoder(int change) {
    if (radio.rx_tx) return;
    const int FREQ_MIN = 1000;
    const int FREQ_MAX = 30000000;
    const int RIT_MIN = -10000;
    const int RIT_MAX = 10000;
    int old_frequency[v_END];
    for (int i=0; i<v_END; i++)
        old_frequency[i] = adj_frequency(i);
    if (radio.rit) {
        int rit = radio.rit_value + 10 * change;
        if (rit < RIT_MIN)
            rit = RIT_MIN;
        else if (rit > RIT_MAX)
            rit = RIT_MAX;
        radio.rit_value = rit;

    } else {
        int frequency = radio.frequency[radio.vfo] + step_values[radio.step] * change;
        if (frequency < FREQ_MIN)
            frequency = FREQ_MIN;
        else if (frequency > FREQ_MAX)
            frequency = FREQ_MAX;
        radio.frequency[radio.vfo] = frequency;
    }
    for (int i=0; i<v_END; i++) {
        if (!radio.split || i == radio.vfo) {
            int new_frequency = adj_frequency(i);
            if (new_frequency != old_frequency[i])
                update_vfo_frequency(i, new_frequency);
        }
    }

}
