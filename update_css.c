#include "update_css.h"

void update_css(GtkWidget *g, CSS_Code css) {
    const void (* add_remove_class[])(GtkStyleContext *, const gchar *) = {
        gtk_style_context_remove_class,
        gtk_style_context_add_class
    };
    const char* CSS_NAME[] = {
        "highlight",    // css_highlight
        "inactive",     // css_inactive
        "tx_inactive",  // css_tx_inactive
        "rx_inactive",  // css_tx_inactive
        "tx_active",    // css_tx_active
        "rx_active"     // css_rx_active
    };
    GtkStyleContext *context = gtk_widget_get_style_context(g); 
    for (int i=0; i<css_END; i++)
        add_remove_class[i==css](context, CSS_NAME[i]);    
}
