#pragma once

#include <gtk/gtk.h>

extern GString *temp_string;

void global_string_init(void);
void global_string_close(void);