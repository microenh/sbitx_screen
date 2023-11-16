#include "debug.h"

bool update_hb_flag = false;

GString *debug_text;

void init_debugger(void) {
    debug_text = g_string_new(NULL);    
}

void close_debugger(void) {
    g_string_free(debug_text, true);
}
