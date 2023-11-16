#pragma once

void hw_init(void);
void hw_close(void);
void hw_set_frequency(int frequency);
void hw_set_tx(bool tx);
void hw_set_af(int level);
void hw_set_if(int level);
void hw_set_filter(gchar filter);