#include <stdbool.h>

#include "global_string.h"

GString
    *temp_string,
    *prefix;

void global_string_init(void) {
    temp_string = g_string_new(NULL);
    prefix = g_string_new(NULL);
}

void global_string_close(void) {
    g_string_free(temp_string, true);
    g_string_free(prefix, true);
}