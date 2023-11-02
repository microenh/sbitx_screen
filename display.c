#include <stdbool.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <time.h>

#include "display.h"
#include "radio.h"
#include "settings.h"
#include "rotary.h"

#define GLADE "sbitx_screen.glade"
#define CSS "main.css"


static void open_glade_and_css(GtkBuilder **builder, GtkCssProvider **css_provider) {
	// look for glade and css files in either the current directory or the one above it
	// i.e. if the file is in a build folder off the main directory
	char temp[4096];
	char prefix[6];
	FILE *f = fopen(GLADE, "r");
	if (f) {
		fclose(f);
		strcpy(prefix, "%s");
	} else {
		strcpy(prefix, "../%s");
	}
	sprintf(temp, prefix, GLADE);
	*builder = gtk_builder_new_from_file(temp);
	sprintf(temp, prefix, CSS);
	*css_provider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(*css_provider, temp, NULL);
}

static GtkLabel
    *lbl_agc,
    *lbl_console,
    *lbl_date,
    *lbl_mode,
    *lbl_record,
    *lbl_rit,
    *lbl_span,
    *lbl_split,
    *lbl_step,
    *lbl_vfo,
    *lbl_vfoa_frequency,
    *lbl_vfob_frequency,
    *lbl_vfoa_mode,
    *lbl_vfob_mode,
    // -----
    *lbl_af,
    *lbl_comp,
    *lbl_high,
    *lbl_if,
    *lbl_low,
    *lbl_mic,
    *lbl_pitch,
    *lbl_power,
    *lbl_wpm,
    *lbl_rx_tx;

static GtkButton
    *btn_af,
    *btn_comp,
    *btn_high,
    *btn_if,
    *btn_low,
    *btn_mic,
    *btn_pitch,
    *btn_power,
    *btn_wpm;


static GtkEntry* ent_command;

static GtkWidget *window;

static GtkStyleContext *highlight[se_END];
static GtkLabel *level[se_END];
static GtkLabel *vfo_frequency[v_END];
static GtkLabel *vfo_mode[v_END];
static GtkStyleContext *vfo_state_context[v_END];
static GtkStyleContext *rx_tx_context;


#define SUB_RESET 10

static volatile int sub_reset = SUB_RESET;

int heartbeat(gpointer data) {
    static int hb_ctr;
    if (hb_ctr) {
        hb_ctr--;
    } else {
        hb_ctr = 8;
        
        char temp[40];

        time_t t;   // not a primitive datatype
        time(&t);

        struct tm *pm;
        pm = gmtime(&t);
        sprintf(temp, "%2d:%02d:%02d", pm->tm_hour, pm->tm_min, pm->tm_sec);
        gtk_label_set_text(lbl_date, temp);
        sub_reset--;
        if (!sub_reset) {
            select_small_encoder(se_af);
            sub_reset = SUB_RESET;
        }
    }
    if (level_ticks) {
        do_sub_encoder(level_ticks);
        level_ticks = 0;
        sub_reset = SUB_RESET;
    }
    if (tuning_ticks) {
        do_main_encoder(tuning_ticks);
        tuning_ticks = 0;
    }
    return true;
}


void init_display(int argc, char **argv) {
	gtk_init(&argc, &argv); // init Gtk

    //---------------------------------------------------------------------
    // establish contact with xml code used to adjust widget settings
    //---------------------------------------------------------------------
 
	GtkBuilder *builder;
	GtkCssProvider *css_provider;
	open_glade_and_css(&builder, &css_provider);

 	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));

    lbl_console = GTK_LABEL(gtk_builder_get_object(builder, "lbl_console"));
    lbl_step = GTK_LABEL(gtk_builder_get_object(builder, "lbl_step"));
    lbl_vfo = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfo"));
    lbl_span = GTK_LABEL(gtk_builder_get_object(builder, "lbl_span"));
    lbl_rit = GTK_LABEL(gtk_builder_get_object(builder, "lbl_rit"));
    lbl_vfob_frequency = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfob_frequency"));
    lbl_vfoa_frequency = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfoa_frequency"));
    lbl_vfob_mode = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfob_mode"));
    lbl_vfoa_mode = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfoa_mode"));
    lbl_date = GTK_LABEL(gtk_builder_get_object(builder, "lbl_date"));
    lbl_record = GTK_LABEL(gtk_builder_get_object(builder, "lbl_record"));
    lbl_split = GTK_LABEL(gtk_builder_get_object(builder, "lbl_split"));

    lbl_power = GTK_LABEL(gtk_builder_get_object(builder, "lbl_power"));
    lbl_mic = GTK_LABEL(gtk_builder_get_object(builder, "lbl_mic"));
    lbl_comp = GTK_LABEL(gtk_builder_get_object(builder, "lbl_comp"));
    lbl_wpm = GTK_LABEL(gtk_builder_get_object(builder, "lbl_wpm"));
    lbl_pitch = GTK_LABEL(gtk_builder_get_object(builder, "lbl_pitch"));
    lbl_mode = GTK_LABEL(gtk_builder_get_object(builder, "lbl_mode"));
    lbl_low = GTK_LABEL(gtk_builder_get_object(builder, "lbl_low"));
    lbl_high = GTK_LABEL(gtk_builder_get_object(builder, "lbl_high"));
    lbl_agc = GTK_LABEL(gtk_builder_get_object(builder, "lbl_agc"));
    lbl_if = GTK_LABEL(gtk_builder_get_object(builder, "lbl_if"));
    lbl_af = GTK_LABEL(gtk_builder_get_object(builder, "lbl_af"));
    lbl_rx_tx = GTK_LABEL(gtk_builder_get_object(builder, "lbl_rx_tx"));

    btn_high = GTK_BUTTON(gtk_builder_get_object(builder, "btn_high"));
    btn_low = GTK_BUTTON(gtk_builder_get_object(builder, "btn_low"));
    btn_af = GTK_BUTTON(gtk_builder_get_object(builder, "btn_af"));
    btn_if = GTK_BUTTON(gtk_builder_get_object(builder, "btn_if"));
    btn_pitch = GTK_BUTTON(gtk_builder_get_object(builder, "btn_pitch"));
    btn_wpm = GTK_BUTTON(gtk_builder_get_object(builder, "btn_wpm"));
    btn_comp = GTK_BUTTON(gtk_builder_get_object(builder, "btn_comp"));
    btn_mic = GTK_BUTTON(gtk_builder_get_object(builder, "btn_mic"));
    btn_power = GTK_BUTTON(gtk_builder_get_object(builder, "btn_power"));

    ent_command = GTK_ENTRY(gtk_builder_get_object(builder, "ent_command"));

	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
								GTK_STYLE_PROVIDER(css_provider),
								GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    highlight[se_af] = gtk_widget_get_style_context(GTK_WIDGET(lbl_af));
    highlight[se_comp] = gtk_widget_get_style_context(GTK_WIDGET(lbl_comp));
    highlight[se_high] = gtk_widget_get_style_context(GTK_WIDGET(lbl_high));
    highlight[se_if] = gtk_widget_get_style_context(GTK_WIDGET(lbl_if));
    highlight[se_low] = gtk_widget_get_style_context(GTK_WIDGET(lbl_low));
    highlight[se_mic] = gtk_widget_get_style_context(GTK_WIDGET(lbl_mic));
    highlight[se_pitch] = gtk_widget_get_style_context(GTK_WIDGET(lbl_pitch));
    highlight[se_power] = gtk_widget_get_style_context(GTK_WIDGET(lbl_power));
    highlight[se_wpm] = gtk_widget_get_style_context(GTK_WIDGET(lbl_wpm));

    level[se_af] = lbl_af;
    level[se_comp] = lbl_comp;
    level[se_high] = lbl_high;
    level[se_if] = lbl_if;
    level[se_low] = lbl_low;
    level[se_mic] = lbl_mic;
    level[se_pitch] = lbl_pitch;
    level[se_power] = lbl_power;
    level[se_wpm] = lbl_wpm;

    vfo_frequency[v_A] = lbl_vfoa_frequency;
    vfo_frequency[v_B] = lbl_vfob_frequency;

    vfo_mode[v_A] = lbl_vfoa_mode;
    vfo_mode[v_B] = lbl_vfob_mode;

    vfo_state_context[v_A] = gtk_widget_get_style_context(GTK_WIDGET(lbl_vfoa_frequency));
    vfo_state_context[v_B] = gtk_widget_get_style_context(GTK_WIDGET(lbl_vfob_frequency));

    rx_tx_context = gtk_widget_get_style_context(GTK_WIDGET(lbl_rx_tx));

    init_gpio_pins();
    g_timeout_add(125, heartbeat, NULL);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);


	gtk_widget_show(window);
	gtk_window_fullscreen(GTK_WINDOW(window));
}


void update_console(char *text) {gtk_label_set_text(lbl_console, text);}
void update_date(char *text) {gtk_label_set_text(lbl_date, text);}

void update_step(Step step) {gtk_label_set_text(lbl_step, steps[step]);}
void update_span(Span span) {gtk_label_set_text(lbl_span, spans[span]);}
void update_mode(Mode mode) {gtk_label_set_text(lbl_mode, modes[mode]);}
void update_agc(Agc agc) {gtk_label_set_text(lbl_agc, agcs[agc]);}
void update_vfo(Vfo vfo) {gtk_label_set_text(lbl_vfo, vfos[vfo]);}


void update_split(bool on) {gtk_label_set_text(lbl_split, off_on[on]);}
void update_rit(bool on) {gtk_label_set_text(lbl_rit, off_on[on]);}
void update_record(bool on) {gtk_label_set_text(lbl_record, off_on[on]);}

void update_level(SubEncoder item, int value) {
    char temp[20];
    sprintf(temp, "%d", value);
    gtk_label_set_text(level[item], temp);    
}


void update_rx_tx(bool rx_tx) {gtk_label_set_text(lbl_rx_tx, rx_txs[rx_tx]);}

static void (* add_class[])(GtkStyleContext *, const gchar *) = {
    gtk_style_context_remove_class,
    gtk_style_context_add_class
};


void enable_highlight(SubEncoder item, bool on) {
    add_class[on](highlight[item], "highlight");
} 


void btn_quit_clicked_cb(GtkButton *b) {
	gtk_main_quit();
}

void update_vfo_frequency(Vfo vfo, int frequency) {
    char temp[20];
    sprintf(temp, "%2d.%03d.%03d", frequency / 1000000, (frequency / 1000) % 1000, frequency % 1000);
    gtk_label_set_text(vfo_frequency[vfo], temp);
}

void update_vfo_mode(Vfo vfo, Mode mode) {
    gtk_label_set_text(vfo_mode[vfo], modes[mode]);
}

const char *VFO_STATE[] = {
    "vfo_inactive",
    "vfo_tx_inactive",
    "vfo_rx_inactive",
    "vfo_tx_active",
    "vfo_rx_active"
};

void update_vfo_state(Vfo vfo, VfoState new_vfoState) {
    static VfoState saved_vfoState[] = {
        vs_END,
        vs_END
    };
    if (new_vfoState == saved_vfoState[vfo]) return;
    if (saved_vfoState[vfo] != vs_END) {
        add_class[false](vfo_state_context[vfo], VFO_STATE[saved_vfoState[vfo]]);
    }
    add_class[true](vfo_state_context[vfo], VFO_STATE[new_vfoState]);
    saved_vfoState[vfo] = new_vfoState;
}


void btn_minimize_clicked_cb(GtkButton *b) {
    gtk_window_iconify(GTK_WINDOW(window));
}

static void call_select_small_encoder(SubEncoder item) {
    sub_reset = SUB_RESET;
    select_small_encoder(item);
}

void update_rx_tx_state(bool tx) {
    static VfoState saved_rx_tx_state = vs_END;
    VfoState new_rx_tx_state = tx ? vs_tx_active : vs_rx_active;
    if (new_rx_tx_state == saved_rx_tx_state) return;
    if (saved_rx_tx_state != vs_END) {
        add_class[false](rx_tx_context, VFO_STATE[saved_rx_tx_state]);
    }
    add_class[true](rx_tx_context, VFO_STATE[new_rx_tx_state]);
    saved_rx_tx_state = new_rx_tx_state;
}
void btn_high_clicked_cb(GtkButton *b) {call_select_small_encoder(se_high);}
void btn_low_clicked_cb(GtkButton *b) {call_select_small_encoder(se_low);}
void btn_af_clicked_cb(GtkButton *b) {call_select_small_encoder(se_af);}
void btn_if_clicked_cb(GtkButton *b) {call_select_small_encoder(se_if);}
void btn_pitch_clicked_cb(GtkButton *b) {call_select_small_encoder(se_pitch);}
void btn_wpm_clicked_cb(GtkButton *b) {call_select_small_encoder(se_wpm);}
void btn_comp_clicked_cb(GtkButton *b) {call_select_small_encoder(se_comp);}
void btn_mic_clicked_cb(GtkButton *b) {call_select_small_encoder(se_mic);}
void btn_power_clicked_cb(GtkButton *b) {call_select_small_encoder(se_power);}

void btn_10m_clicked_cb(GtkButton *b) {do_band(b_10m);}
void btn_12m_clicked_cb(GtkButton *b) {do_band(b_12m);}
void btn_15m_clicked_cb(GtkButton *b) {do_band(b_15m);}
void btn_17m_clicked_cb(GtkButton *b) {do_band(b_17m);}
void btn_20m_clicked_cb(GtkButton *b) {do_band(b_20m);}
void btn_30m_clicked_cb(GtkButton *b) {do_band(b_30m);}
void btn_40m_clicked_cb(GtkButton *b) {do_band(b_40m);}
void btn_80m_clicked_cb(GtkButton *b) {do_band(b_80m);}

void btn_agc_clicked_cb(GtkButton *b) {do_agc();}
void btn_mode_clicked_cb(GtkButton *b) {do_mode();}
void btn_split_clicked_cb(GtkButton *b) {do_split();}
void btn_record_clicked_cb(GtkButton *b) {do_record();}
void btn_rx_tx_clicked_cb(GtkButton *b) {do_rx_tx();}
void btn_rit_clicked_cb(GtkButton *b) {do_rit();}
void btn_span_clicked_cb(GtkButton *b) {do_span();}
void btn_vfo_clicked_cb(GtkButton *b) {do_vfo();}
void btn_step_clicked_cb(GtkButton *b) {do_step();}