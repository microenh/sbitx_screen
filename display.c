#include <ctype.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "console.h"
#include "debug.h"
#include "display.h"
#include "global_string.h"
#include "radio_state.h"
#include "rotary.h"
#include "settings.h"
#include "tmate2/tmate2.h"
#include "update_css.h"

static const gchar * const GLADE = "sbitx_screen.glade";
static const gchar * const CSS = "main.css";

G_MODULE_EXPORT void btn_quit_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_minimize_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_high_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_low_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_af_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_if_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_pitch_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_wpm_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_comp_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_mic_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_power_clicked_cb(GtkButton *b);

G_MODULE_EXPORT void btn_10m_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_12m_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_15m_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_17m_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_20m_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_30m_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_40m_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_80m_clicked_cb(GtkButton *b);

G_MODULE_EXPORT void btn_agc_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_mode_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_split_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_record_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_rx_tx_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_rit_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_span_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_vfo_clicked_cb(GtkButton *b);
G_MODULE_EXPORT void btn_step_clicked_cb(GtkButton *b);

G_MODULE_EXPORT void ent_command_activate_cb(GtkEntry *e);

typedef enum _offOn {
    o_off,
    o_on,
    o_END
} OffOn;

static void open_glade_and_css(GtkBuilder **builder, GtkCssProvider **css_provider) {
	// look for glade and css files in either the current directory or the one above it
	// i.e. if the file is in a build folder off the main directory
	FILE *f = fopen(GLADE, "r");
	if (f) {
		fclose(f);
        g_string_assign(prefix, "%s");
	} else {
        g_string_assign(prefix, "../%s");
	}
	g_string_printf(temp_string, prefix->str, GLADE);
	*builder = gtk_builder_new_from_file(temp_string->str);
	g_string_printf(temp_string, prefix->str, CSS);
	*css_provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(*css_provider, temp_string->str, NULL);
}

static GtkWidget *window;

static GtkLabel
    *lbl_agc,
    *lbl_date,
    *lbl_mode,
    *lbl_record,
    *lbl_rit,
    *lbl_span,
    *lbl_split,
    *lbl_step,
    *lbl_tx,
    *lbl_vfo,
    *lbl_heartbeat,
    *level[se_END],
    *vfo_frequency[v_END],
    *vfo_mode[v_END];

static GtkButton
    *btn_tx;

GtkEntry* ent_command;
GtkTextBuffer *tb_console;
static GtkDrawingArea* dwg_panafall;

GString *temp;

#define SUB_RESET 10

static volatile int sub_reset = SUB_RESET;
static void call_select_sub_encoder(SubEncoder item);

int heartbeat(gpointer data) {
    static int hb_ctr;
    if (hb_ctr) {
        hb_ctr--;
    } else {
        hb_ctr = 8;
        debug_check();
        
        time_t t;   // not a primitive datatype
        time(&t);

        struct tm *pm;
        pm = gmtime(&t);
        g_string_printf(temp_string, "%2d:%02d:%02d", pm->tm_hour, pm->tm_min, pm->tm_sec);
        gtk_label_set_text(lbl_date, temp_string->str);
        if (sub_reset) {
            if (!--sub_reset) {
                call_select_sub_encoder(se_af);
            }
        }
    }
    tmate2_tick();

    if (level_ticks) {
        do_sub_encoder_inc(level_ticks);
        level_ticks = 0;
        if (sub_reset)
            sub_reset = SUB_RESET;
    }
    if (tuning_ticks) {
        do_main_encoder(tuning_ticks);
        tuning_ticks = 0;
    }
    return true;
}


void display_init(int argc, char **argv) {
	gtk_init(&argc, &argv); // init Gtk

    //---------------------------------------------------------------------
    // establish contact with xml code used to adjust widget settings
    //---------------------------------------------------------------------
 
	GtkBuilder *builder;
	GtkCssProvider *css_provider;
	open_glade_and_css(&builder, &css_provider);
	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
								GTK_STYLE_PROVIDER(css_provider),
								GTK_STYLE_PROVIDER_PRIORITY_USER);
    
 	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    lbl_agc = GTK_LABEL(gtk_builder_get_object(builder, "lbl_agc"));
    lbl_date = GTK_LABEL(gtk_builder_get_object(builder, "lbl_date"));
    lbl_mode = GTK_LABEL(gtk_builder_get_object(builder, "lbl_mode"));
    lbl_record = GTK_LABEL(gtk_builder_get_object(builder, "lbl_record"));
    lbl_rit = GTK_LABEL(gtk_builder_get_object(builder, "lbl_rit"));
    lbl_span = GTK_LABEL(gtk_builder_get_object(builder, "lbl_span"));
    lbl_split = GTK_LABEL(gtk_builder_get_object(builder, "lbl_split"));
    lbl_step = GTK_LABEL(gtk_builder_get_object(builder, "lbl_step"));
    lbl_tx = GTK_LABEL(gtk_builder_get_object(builder, "lbl_rx_tx"));
    lbl_vfo = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfo"));
    lbl_heartbeat = GTK_LABEL(gtk_builder_get_object(builder, "lbl_heartbeat"));

    ent_command = GTK_ENTRY(gtk_builder_get_object(builder, "ent_command"));
    GtkTextView *txv_console = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "txv_console"));
    tb_console = gtk_text_view_get_buffer(txv_console);
 
    level[se_af] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_af"));
    level[se_comp] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_comp"));
    level[se_high] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_high"));
    level[se_if] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_if"));
    level[se_low] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_low"));
    level[se_mic] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_mic"));
    level[se_pitch] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_pitch"));
    level[se_power] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_power"));
    level[se_wpm] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_wpm"));

    vfo_frequency[v_A] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfoa_frequency"));
    vfo_frequency[v_B] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfob_frequency"));

    vfo_mode[v_A] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfoa_mode"));
    vfo_mode[v_B] = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfob_mode"));

    dwg_panafall = GTK_DRAWING_AREA(gtk_builder_get_object(builder, "dwg_panafall"));

    btn_tx = GTK_BUTTON(gtk_builder_get_object(builder, "btn_tx"));

    g_timeout_add(125, heartbeat, NULL);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

	gtk_widget_show(window);
    #ifndef NO_HARDWARE
	gtk_window_fullscreen(GTK_WINDOW(window));
    #endif
}

void display_close(void) {
}

void update_date(char *text) {gtk_label_set_text(lbl_date, text);}

void update_step(Step step) {
    static Step saved = s_END;
    if (step != saved) {
        saved = step;
        gtk_label_set_text(lbl_step, steps[step]);
    }
}

void update_debug_heartbeat(const gchar * const text) {
    gtk_label_set_text(lbl_heartbeat, text);
}

void update_span(Span span)
{
    static Span saved = sp_END;
    if (span != saved) {
        saved = span;
        gtk_label_set_text(lbl_span, spans[span]);
    }
}

void update_mode(Mode mode)
{
    static Mode saved = m_END;
    if (mode != saved) {
        saved = mode;
        gtk_label_set_text(lbl_mode, modes[mode]);
    }
}

void update_agc(Agc agc) {
    static Agc saved = a_END;
    if (agc != saved) {
        saved = agc;
        gtk_label_set_text(lbl_agc, agcs[agc]);
    }
}

void update_vfo(Vfo vfo) {
    static Vfo saved = v_END;
    if (vfo != saved) {
        saved = vfo;
        gtk_label_set_text(lbl_vfo, vfos[vfo]);}
    }


void update_split(bool on) {
    static OffOn saved = o_END;
    if (on != saved) {
        saved = on;
        gtk_label_set_text(lbl_split, off_on[on]);
    }
}

void update_rit(bool on) {
    static OffOn saved = o_END;
    if (on != saved) {
        saved = on;
        gtk_label_set_text(lbl_rit, off_on[on]);
    }
}

void update_record(bool on) {
    static OffOn saved = o_END;
    if (on != saved) {
        saved = on;
        gtk_label_set_text(lbl_record, off_on[on]);
    }
}

void update_level(SubEncoder item, int value) {
    static int saved[se_END] = {
        -1, -1, -1, -1, -1, -1, -1, -1, -1
    };
    if (value != saved[item]) {
        saved[item] = value;
        char temp[20];
        sprintf(temp, "%d", value);
        gtk_label_set_text(level[item], temp); 
    }   
}


void update_level_highlight(SubEncoder item, bool on) {
    static OffOn saved[se_END] = {
        o_END, o_END, o_END, o_END, o_END, o_END, o_END, o_END, o_END
    };
    if (on != saved[item]) {
        saved[item] = on;
        update_css(GTK_WIDGET(level[item]), on ? css_highlight : css_END);
    }
} 

void update_vfo_state(Vfo vfo, VfoState state) {
    static VfoState saved[v_END] = {
        vs_END, vs_END
    };
    if (state != saved[vfo]) {
        const CSS_Code css_lu[] = {
            css_inactive,
            css_tx_inactive,
            css_rx_inactive,
            css_tx_active,
            css_rx_active
        };
        saved[vfo] = state;
        update_css(GTK_WIDGET(vfo_frequency[vfo]), css_lu[state]);
    }
}

void update_tx(bool tx) {
    static OffOn saved = o_END;
    if (tx != saved) {
        saved = tx;
        gtk_label_set_text(lbl_tx, rx_txs[tx]);
        update_css(GTK_WIDGET(lbl_tx), tx ? css_tx_active : css_rx_active);
        if (tx)
            call_select_sub_encoder(se_af);    
    }
}

void update_vfo_frequency(Vfo vfo, int frequency) {
    static int saved[v_END] = {
        -1, -1
    };
    if (frequency != saved[vfo]) {
        display_main_number(frequency);
        char temp[20];
        saved[vfo] = frequency;
        sprintf(temp, "%2d.%03d.%02d", frequency / 1000000, (frequency / 1000) % 1000, (frequency % 1000) / 10);
        gtk_label_set_text(vfo_frequency[vfo], temp);
    }
}

void update_vfo_mode(Vfo vfo, Mode mode) {
    static Mode saved[v_END] = {
        m_END, m_END
    };
    if (mode != saved[vfo]) {
        saved[vfo] = mode;
        gtk_label_set_text(vfo_mode[vfo], modes[mode]);
    }
}

void btn_quit_clicked_cb(GtkButton *b) {
	gtk_main_quit();
}

void btn_minimize_clicked_cb(GtkButton *b) {
    gtk_window_iconify(GTK_WINDOW(window));
}

static void call_select_sub_encoder(SubEncoder item) {
    static SubEncoder saved = se_END;
    if (item != saved) {
        saved = item;
        sub_reset = item == se_af ? 0 : SUB_RESET;
        select_sub_encoder(item);
    }
}

void update_tx_enable(bool enable) {
    gtk_widget_set_sensitive(GTK_WIDGET(btn_tx), enable); 
    update_css(GTK_WIDGET(lbl_tx), enable ? css_rx_active : css_rx_inactive);   
}

void btn_high_clicked_cb(GtkButton *b) {call_select_sub_encoder(se_high);}
void btn_low_clicked_cb(GtkButton *b) {call_select_sub_encoder(se_low);}
void btn_af_clicked_cb(GtkButton *b) {call_select_sub_encoder(se_af);}
void btn_if_clicked_cb(GtkButton *b) {call_select_sub_encoder(se_if);}
void btn_pitch_clicked_cb(GtkButton *b) {call_select_sub_encoder(se_pitch);}
void btn_wpm_clicked_cb(GtkButton *b) {call_select_sub_encoder(se_wpm);}
void btn_comp_clicked_cb(GtkButton *b) {call_select_sub_encoder(se_comp);}
void btn_mic_clicked_cb(GtkButton *b) {call_select_sub_encoder(se_mic);}
void btn_power_clicked_cb(GtkButton *b) {call_select_sub_encoder(se_power);}

void btn_10m_clicked_cb(GtkButton *b) {do_band(b_10m);}
void btn_12m_clicked_cb(GtkButton *b) {do_band(b_12m);}
void btn_15m_clicked_cb(GtkButton *b) {do_band(b_15m);}
void btn_17m_clicked_cb(GtkButton *b) {do_band(b_17m);}
void btn_20m_clicked_cb(GtkButton *b) {do_band(b_20m);}
void btn_30m_clicked_cb(GtkButton *b) {do_band(b_30m);}
void btn_40m_clicked_cb(GtkButton *b) {do_band(b_40m);}
void btn_80m_clicked_cb(GtkButton *b) {do_band(b_80m);}

void btn_agc_clicked_cb(GtkButton *b) {do_agc_inc();}
void btn_mode_clicked_cb(GtkButton *b) {do_mode_inc();}
void btn_split_clicked_cb(GtkButton *b) {do_split_inc();}
void btn_record_clicked_cb(GtkButton *b) {do_record_inc();}
void btn_rx_tx_clicked_cb(GtkButton *b) {do_tx_inc();}
void btn_rit_clicked_cb(GtkButton *b) {do_rit_inc();}
void btn_span_clicked_cb(GtkButton *b) {do_span_inc();}
void btn_vfo_clicked_cb(GtkButton *b) {do_vfo_inc();}
void btn_step_clicked_cb(GtkButton *b) {do_step_inc();}

void ent_command_activate_cb(GtkEntry *e) {do_console_entry();}

