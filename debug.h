#pragma once

#include <gtk/gtk.h>
#include <stdbool.h>

void debug_init(void);
void debug_close(void);

// can be called by other threads
void debug_heartbeat(void);
void debug_text(const gchar * const text);
void debug_printf(const char *fmt, ...);

// use only by display.c
void debug_check(void);
