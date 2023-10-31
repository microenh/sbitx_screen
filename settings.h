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
