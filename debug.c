#include <stdbool.h>

#include "console.h"
#include "display.h"
#include "debug.h"
#include "global_string.h"

bool update_hb_flag = false;

const int MAX_LINE = 25;

static GString *d_text;

void debug_init(void) {
    d_text = g_string_new(NULL);    
}

void debug_close(void) {
    g_string_free(d_text, true);
}

void debug_text(const gchar const * text) {
    g_string_assign(d_text, text);
}

void debug_heartbeat(void) {
    update_hb_flag = true;
}

void debug_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    g_string_vprintf(temp_string, fmt, args);
    va_end(args);
    g_string_truncate(temp_string, MAX_LINE);
    if (d_text->len)
        g_string_append(d_text, "\r\n");
    g_string_append(d_text, temp_string->str);
}

// use only by display.c
// called in main thread - safe to update display
void debug_check(void) {
    static bool state = false;
    if (update_hb_flag) {
        update_hb_flag = false;
        state = !state;
        update_debug_heartbeat(state ? "*" : "");
    }
    if (d_text->len) {
        update_console(d_text->str);
        g_string_truncate(d_text, 0);
    }
}
