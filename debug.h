#pragma once

#include <stdbool.h>
#include <gtk/gtk.h>

extern bool update_hb_flag;
extern GString *debug_text;

void init_debugger(void);
void close_debugger(void);
