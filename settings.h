#pragma once

typedef enum _step {
    s_10Hz,
    s_100Hz,
    s_1kHz,
    s_10kHz,
    s_100kHz,
    s_1MHz,
    s_END
} Step;

typedef enum _span {
    sp_1250Hz,
    sp_2500Hz,
    sp_5kHz,
    sp_10kHz,
    sp_20kHz,
    sp_24kHz,
    sp_END
} Span;

typedef enum _mode {
    m_lsb,
    m_usb,
    m_cwr,
    m_cw,
    m_END
} Mode;

typedef enum _agc {
    a_off,
    a_fast,
    a_medium,
    a_slow,
    a_END
} Agc;

typedef enum _vfo {
    v_A,
    v_B,
    v_END
} Vfo;

typedef enum _subEncoder {
    se_af,
    se_comp,
    se_high,
    se_if,
    se_low,
    se_mic,
    se_pitch,
    se_power,
    se_wpm,
    se_END
} SubEncoder;

typedef enum _vfoState {
    vs_inactive,
    vs_tx_inactive,
    vs_rx_inactive,
    vs_tx_active,
    vs_rx_active,
    vs_END
} VfoState;

typedef enum _band {
    b_10m,
    b_12m,
    b_15m,
    b_17m,
    b_20m,
    b_30m,
    b_40m,
    b_80m,
    b_END
} Band;