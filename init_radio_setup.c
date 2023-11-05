#include "radio_state.h"

void initial_radio_settings(Radio *radio) {
    int level[3][se_END];
    for (int i=0; i<se_END; i++)
        for (int j=0; j<3; j++)
            level[j][i] = 0;

    level[0][se_low] = 350;
    level[0][se_high] = 850;
    level[0][se_pitch] = 600;
    level[0][se_wpm] = 13;
    level[1][se_low] = 300;
    level[1][se_high] = 2800;
    level[1][se_pitch] = 600;
    level[1][se_wpm] = 13;
    level[2][se_low] = 0;
    level[2][se_high] = 4000;
    level[2][se_pitch] = 1500;
    level[2][se_wpm] = 13;

    MiscSettings miscSettings = {
        .rit = false,
        .rit_value = 0,
        .split = false,
        .span = sp_10kHz,
        .step = s_10Hz
    };

    Agc agcs[3] = {a_fast, a_slow, a_off};
    Mode modes[3] = {m_cw, m_lsb, m_data};

    VfoData vfoData;
    int frequencies[b_END][3] = {
        { 3530000,  3815000,  3573000},
        { 7030000,  7215000,  7074000},
        {10105000, 10110000, 10136000},
        {14030000, 14210000, 14074000},
        {18069000, 18167000, 18100000},
        {21030000, 21220000, 21074000},
        {24900000, 24980000, 24915000},
        {28030000, 24330000, 28074000}
    };

    Step steps[3] = {s_10Hz, s_100Hz, s_10Hz};

    for (int b=0; b<b_END; b++) {
        if (b == b_30m)
            modes[1] = m_cw;
        else if (b == b_20m)
            modes[1] = m_usb;
        for (int s=0; s<3; s++) {
            miscSettings.step = steps[s];
            radio->bandStack[b].bandStackEntry[s].miscSettings = miscSettings;
            for (int v=0; v<v_END; v++) {
                radio->bandStack[b].bandStackEntry[s].vfoData[v].agc = agcs[s];
                radio->bandStack[b].bandStackEntry[s].vfoData[v].mode = modes[s];
                radio->bandStack[b].bandStackEntry[s].vfoData[v].frequency = frequencies[b][s];
                for (int i=0; i<se_END; i++)
                    radio->bandStack[b].bandStackEntry[s].vfoData[v].level[i] = level[s][i]; 
            }
        }       
    }

    const int sr=0;
    const int bd=b_80m;
    for (int i=0; i<v_END; i++) {
        radio->vfoData[i] = radio->bandStack[bd].bandStackEntry[sr].vfoData[i];
    }
    radio->miscSettings = radio->bandStack[bd].bandStackEntry[sr].miscSettings;
    radio->vfo = v_A;
    radio->subEncoder = se_af;
    radio->tx = false;
    radio->record = false;
}
