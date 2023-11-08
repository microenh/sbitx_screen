#include <stdio.h>
#include "radio_state.h"
#include "display.h"

static Radio radio;

static int adj;

#define SETTINGS "settings.dat"

char prefix[6];

static Band get_band(void) {
    int f = radio.vfoData[radio.vfo].frequency;
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

static void update_vfo_states(void) {
    Vfo sel, unsel;
    VfoState selState, unselState;
    sel = radio.vfo;
    unsel = (radio.vfo + 1) % v_END;
    bool tx = radio.tx;
    if (radio.miscSettings.split) {
        selState = tx ? vs_rx_inactive : vs_rx_active;
        unselState = tx ? vs_tx_active : vs_tx_inactive;
    } else {
        selState = tx ? vs_tx_active : vs_rx_active;
        unselState = vs_inactive;
    }
    update_vfo_state(sel, selState);
    update_vfo_state(unsel, unselState);
}

static void update_display(void) {
    update_agc(radio.vfoData[radio.vfo].agc);
    update_mode(radio.vfoData[radio.vfo].mode);
    update_span(radio.miscSettings.span);
    update_vfo(radio.vfo);
    update_step(radio.miscSettings.step);
    update_rit(radio.miscSettings.rit);
    update_split(radio.miscSettings.split);
    update_record(radio.record);
    update_tx(radio.tx);
    for (int i=0; i<se_END; i++) {
        update_level(i, radio.vfoData[radio.vfo].level[i]);
        update_level_highlight(i, i==radio.subEncoder);
    }
    for (int i=0; i<v_END; i++) {
        update_vfo_frequency(i, radio.vfoData[i].frequency);
        update_vfo_mode(i, radio.vfoData[i].mode);
    }
    update_vfo_states();
}

void do_band(Band band) {
    if (radio.tx) return;
    Band cur_band = get_band();
    // same band: swap and increment tos
    if (cur_band == band) {
        int tos = radio.bandStack[cur_band].current;
        for (int i=0; i<v_END; i++) {
            VfoData temp_vfoData;
            temp_vfoData = radio.bandStack[band].bandStackEntry[tos].vfoData[i];
            radio.bandStack[band].bandStackEntry[tos].vfoData[i] = radio.vfoData[i];
            radio.vfoData[i] = temp_vfoData;
        }
        MiscSettings temp_miscSettings = radio.bandStack[band].bandStackEntry[tos].miscSettings;
        radio.bandStack[band].bandStackEntry[tos].miscSettings = radio.miscSettings;
        radio.miscSettings = temp_miscSettings;
        tos++;
        if (tos >= BAND_STACK_SIZE)
            tos = 0;
        radio.bandStack[band].current = tos;           
    } else {
        if (cur_band != b_END) {
            // save to current tos
            int sr = radio.bandStack[cur_band].current;
            for (int i=0; i<v_END; i++)
                radio.bandStack[cur_band].bandStackEntry[sr].vfoData[i] = radio.vfoData[i];
            radio.bandStack[cur_band].bandStackEntry[sr].miscSettings = radio.miscSettings;
        }
        int sr = radio.bandStack[cur_band].current;
        for (int i=0; i<v_END; i++)
            radio.vfoData[i] = radio.bandStack[band].bandStackEntry[sr].vfoData[i];
        radio.miscSettings = radio.bandStack[band].bandStackEntry[sr].miscSettings;
        sr++;
        if (sr >= BAND_STACK_SIZE)
            sr = 0;
        radio.bandStack[band].current = sr;
    }           
    radio.subEncoder = se_af;
    radio.tx = false;
    radio.record = false;
    update_display();
}

static void load_settings(void) {
    char temp[25];
    sprintf(temp, prefix, SETTINGS);
    FILE *f = fopen(temp, "r");
    if (f) {
        fread(&radio, sizeof(radio), 1, f);
        fclose(f);
    } else {
        initial_radio_settings(&radio);
    }

}

void save_settings(void) {
    char temp[25];
    sprintf(temp, prefix, SETTINGS);
    FILE *f = fopen(temp, "w");
    fwrite(&radio, sizeof(radio), 1, f);
    fclose(f);
}


//                                    af, comp, high,  if,  low, mic, pitch, power, wpm
static const int subEncoderMin[]  = {  0,    0,    0,   0,    0,   0,   300,     0,   5};
static const int subEncoderInit[] = { 50,    0, 3000,  50,  200,  50,   600,     0,  13};
static const int subEncoderMax[]  = {100,  100, 4000, 100, 1000, 100,  1500,   100,  40};
static const int subEncoderStep[] = {  1,    1,   50,   1,   50,   1,    10,     1,   1};

void init_radio(void) {
    load_settings();    
    update_display();
}

void do_agc(void) {
    if (radio.tx) return;
    Agc agc = radio.vfoData[radio.vfo].agc;
    agc++;
    if (agc >= a_END)
        agc = 0;
    radio.vfoData[radio.vfo].agc = agc;    
    update_agc(agc);
}

void do_mode(void) {
    if (radio.tx) return;
    Mode mode = radio.vfoData[radio.vfo].mode;
    mode++;
    if (mode >= m_END)
        mode = 0;
    radio.vfoData[radio.vfo].mode = mode;
    update_mode(mode);
    update_vfo_mode(radio.vfo, mode);
}

void do_span(void) {
    if (radio.tx) return;
    Span span = radio.miscSettings.span;
    span++;
    if (span >= sp_END)
        span = 0;
    radio.miscSettings.span = span;
    update_span(span);
}

void do_vfo(void) {
    if (radio.tx) return;
    radio.vfo++;
    if (radio.vfo >= v_END)
        radio.vfo = 0;
    update_vfo(radio.vfo);
    update_vfo_states();
}

void do_step(void) {
    if (radio.tx) return;
    Step step = radio.miscSettings.step;
    step++;
    if (step >= s_END)
        step = 0;
    radio.miscSettings.step = step;
    update_step(step);
    adj = radio.vfoData[radio.vfo].frequency % step_values[step];
}

static int adj_frequency(Vfo vfo){
    int frequency = radio.vfoData[vfo].frequency;
    if (radio.miscSettings.rit)
        frequency += radio.miscSettings.rit_value;
    return frequency;
}

void do_rit(void) {
    if (radio.tx) return;
    radio.miscSettings.rit = !radio.miscSettings.rit;
    update_rit(radio.miscSettings.rit);
    if (radio.miscSettings.rit_value) {
        for (int i=0; i<v_END; i++)
            if (!radio.miscSettings.split || i == radio.vfo)
                update_vfo_frequency(i, adj_frequency(i));        
    }
}

void do_split(void) {
    if (radio.tx) return;
    radio.miscSettings.split = !radio.miscSettings.split;
    update_split(radio.miscSettings.split);
    update_vfo_states();
}

void do_record(void) {
    radio.record = !radio.record;
    update_record(radio.record);
}

void do_tx(void) {
    radio.tx = !radio.tx;
    update_tx(radio.tx);
    update_vfo_states();
}

void select_sub_encoder(SubEncoder item) {
    if (item != radio.subEncoder) {
        update_level_highlight(radio.subEncoder, false);
        radio.subEncoder = item;
        update_level_highlight(radio.subEncoder, true);  
    }  
}

void do_sub_encoder(int change) {
    int rse = radio.subEncoder;
    int min = subEncoderMin[rse];
    int max = subEncoderMax[rse];
    int step = subEncoderStep[rse];
    int i = radio.vfoData[radio.vfo].level[radio.subEncoder] + change * step;
    if (rse == se_high) {
        min = radio.vfoData[radio.vfo].level[se_low] + step;
    }
    if (rse == se_low) {
        max = radio.vfoData[radio.vfo].level[se_high] - step;
    }
    if (i < min)
        i = min;
    else if (i > max)
        i = max;    
    if (i != radio.vfoData[radio.vfo].level[rse]) {
        radio.vfoData[radio.vfo].level[rse] = i;
        update_level(rse, i);
    }
}

const int FREQ_MIN = 1000;
const int FREQ_MAX = 30000000;
const int RIT_MIN = -10000;
const int RIT_MAX = 10000;


void do_frequency(int frequency) {
    if (radio.tx) return;
    radio.miscSettings.rit = false;
    radio.miscSettings.rit_value = 0;
    if (frequency < FREQ_MIN)
        frequency = FREQ_MIN;
    else if (frequency > FREQ_MAX)
        frequency = FREQ_MAX;
    radio.vfoData[radio.vfo].frequency = frequency;
    update_vfo_frequency(radio.vfo, frequency);
    update_rit(radio.miscSettings.rit);
}

void do_main_encoder(int change) {
    if (radio.tx) return;
    if (radio.miscSettings.rit) {
        int rit = radio.miscSettings.rit_value + 10 * change;
        if (rit < RIT_MIN)
            rit = RIT_MIN;
        else if (rit > RIT_MAX)
            rit = RIT_MAX;
        radio.miscSettings.rit_value = rit;
    } else {
        int frequency = radio.vfoData[radio.vfo].frequency - adj + step_values[radio.miscSettings.step] * change;
        adj = 0;
        if (frequency < FREQ_MIN)
            frequency = FREQ_MIN;
        else if (frequency > FREQ_MAX)
            frequency = FREQ_MAX;
        radio.vfoData[radio.vfo].frequency = frequency;
    }
    for (int i=0; i<v_END; i++) {
        if (!radio.miscSettings.split || i == radio.vfo) {
            update_vfo_frequency(i, adj_frequency(i));
        }
    }
}
