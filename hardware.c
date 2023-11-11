#include <stdbool.h>
#include "hardware.h"
#include "radio_state.h"
#include "si5351.h"

void hw_set_tx(bool tx) {}

const int BFO_FREQ = 40035000;
const int BFO_OFFSET = 24000;
const int TUNING_SHIFT = 0;

void hw_set_frequency(int frequency) {
    int freq_adj;
    switch (get_mode()) {
        case m_cw:
            freq_adj = -get_rx_pitch();
            break;
        case m_cwr:
            freq_adj = get_rx_pitch();
            break;
        default:
            freq_adj = 0;
            break;
    }
    si5351bx_setfreq(2, frequency + freq_adj + BFO_FREQ - BFO_OFFSET + TUNING_SHIFT);
}
