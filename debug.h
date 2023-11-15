#pragma once

#include <stdbool.h>
#include <gtk/gtk.h>

extern bool update_hb_flag;

void init_debugger(void);
void close_debugger(void);
void debug_message(const gchar * const text);