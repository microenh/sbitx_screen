#pragma once

#include <gtk/gtk.h>
#include <gtk/gtkx.h>

typedef enum _css {
    css_highlight,
    css_inactive,
    css_tx_inactive,
    css_rx_inactive,
    css_tx_active,
    css_rx_active,
    css_END
} CSS_Code;

void update_css(GtkWidget *g, CSS_Code css);