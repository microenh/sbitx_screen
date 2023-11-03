#include <stdbool.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <time.h>

#include "display.h"
#include "radio.h"
#include "settings.h"
#include "rotary.h"
#include "update_css.h"

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
    *lbl_rx_tx;

static GtkEntry* ent_command;

static GtkWidget *window;

static GtkLabel *level[se_END];
static GtkLabel *vfo_frequency[v_END];
static GtkLabel *vfo_mode[v_END];


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
            select_sub_encoder(se_af);
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
    GtkLabel *lbl_vfob_frequency = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfob_frequency"));
    GtkLabel *lbl_vfoa_frequency = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfoa_frequency"));
    GtkLabel *lbl_vfob_mode = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfob_mode"));
    GtkLabel *lbl_vfoa_mode = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfoa_mode"));
    lbl_date = GTK_LABEL(gtk_builder_get_object(builder, "lbl_date"));
    lbl_record = GTK_LABEL(gtk_builder_get_object(builder, "lbl_record"));
    lbl_split = GTK_LABEL(gtk_builder_get_object(builder, "lbl_split"));

    GtkLabel *lbl_power = GTK_LABEL(gtk_builder_get_object(builder, "lbl_power"));
    GtkLabel *lbl_mic = GTK_LABEL(gtk_builder_get_object(builder, "lbl_mic"));
    GtkLabel *lbl_comp = GTK_LABEL(gtk_builder_get_object(builder, "lbl_comp"));
    GtkLabel *lbl_wpm = GTK_LABEL(gtk_builder_get_object(builder, "lbl_wpm"));
    GtkLabel *lbl_pitch = GTK_LABEL(gtk_builder_get_object(builder, "lbl_pitch"));
    lbl_mode = GTK_LABEL(gtk_builder_get_object(builder, "lbl_mode"));
    GtkLabel *lbl_low = GTK_LABEL(gtk_builder_get_object(builder, "lbl_low"));
    GtkLabel *lbl_high = GTK_LABEL(gtk_builder_get_object(builder, "lbl_high"));
    lbl_agc = GTK_LABEL(gtk_builder_get_object(builder, "lbl_agc"));
    GtkLabel *lbl_if = GTK_LABEL(gtk_builder_get_object(builder, "lbl_if"));
    GtkLabel *lbl_af = GTK_LABEL(gtk_builder_get_object(builder, "lbl_af"));
    lbl_rx_tx = GTK_LABEL(gtk_builder_get_object(builder, "lbl_rx_tx"));

    ent_command = GTK_ENTRY(gtk_builder_get_object(builder, "ent_command"));

	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
								GTK_STYLE_PROVIDER(css_provider),
								GTK_STYLE_PROVIDER_PRIORITY_USER);
    
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


// Handle CSS changes

void enable_highlight(SubEncoder item, bool on) {
    update_css(GTK_WIDGET(level[item]), on ? css_highlight : css_END);
} 

void update_vfo_state(Vfo vfo, VfoState state) {
    const CSS_Code css_lu[] = {
        css_inactive,
        css_tx_inactive,
        css_rx_inactive,
        css_tx_active,
        css_rx_active
    };
    update_css(GTK_WIDGET(vfo_frequency[vfo]), css_lu[state]);
}

void update_rx_tx_state(bool tx) {
    update_css(GTK_WIDGET(lbl_rx_tx), tx ? css_tx_active : css_rx_active);
}

// end CSS changes


static void call_select_sub_encoder(SubEncoder item) {
    sub_reset = SUB_RESET;
    select_sub_encoder(item);
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

void btn_minimize_clicked_cb(GtkButton *b) {
    gtk_window_iconify(GTK_WINDOW(window));
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

void btn_agc_clicked_cb(GtkButton *b) {do_agc();}
void btn_mode_clicked_cb(GtkButton *b) {do_mode();}
void btn_split_clicked_cb(GtkButton *b) {do_split();}
void btn_record_clicked_cb(GtkButton *b) {do_record();}
void btn_rx_tx_clicked_cb(GtkButton *b) {do_rx_tx();}
void btn_rit_clicked_cb(GtkButton *b) {do_rit();}
void btn_span_clicked_cb(GtkButton *b) {do_span();}
void btn_vfo_clicked_cb(GtkButton *b) {do_vfo();}
void btn_step_clicked_cb(GtkButton *b) {do_step();}