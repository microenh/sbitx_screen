#include <stdbool.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>

#include "display.h"
#include "radio.h"
#include "settings.h"

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
    *lbl_console,
    *lbl_step,
    *lbl_vfo,
    *lbl_span,
    *lbl_rit,
    *lbl_vfob,
    *lbl_vfoa,
    *lbl_date,
    *lbl_power,
    *lbl_record,
    *lbl_mic,
    *lbl_comp,
    *lbl_wpm,
    *lbl_pitch,
    *lbl_split,
    *lbl_mode,
    *lbl_low,
    *lbl_high,
    *lbl_agc,
    *lbl_if,
    *lbl_af;

static GtkButton
    *btn_high,
    *btn_low,
    *btn_af,
    *btn_if,
    *btn_pitch,
    *btn_wpm,
    *btn_comp,
    *btn_mic,
    *btn_power,
    *btn_rx_tx;


static GtkEntry* ent_command;

static GtkWidget *window;

static GtkStyleContext *highlight[se_END];

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
    lbl_vfob = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfob"));
    lbl_vfoa = GTK_LABEL(gtk_builder_get_object(builder, "lbl_vfoa"));
    lbl_date = GTK_LABEL(gtk_builder_get_object(builder, "lbl_date"));
    lbl_power = GTK_LABEL(gtk_builder_get_object(builder, "lbl_power"));
    lbl_record = GTK_LABEL(gtk_builder_get_object(builder, "lbl_record"));
    lbl_mic = GTK_LABEL(gtk_builder_get_object(builder, "lbl_mic"));
    lbl_comp = GTK_LABEL(gtk_builder_get_object(builder, "lbl_comp"));
    lbl_wpm = GTK_LABEL(gtk_builder_get_object(builder, "lbl_wpm"));
    lbl_pitch = GTK_LABEL(gtk_builder_get_object(builder, "lbl_pitch"));
    lbl_split = GTK_LABEL(gtk_builder_get_object(builder, "lbl_split"));
    lbl_mode = GTK_LABEL(gtk_builder_get_object(builder, "lbl_mode"));
    lbl_low = GTK_LABEL(gtk_builder_get_object(builder, "lbl_low"));
    lbl_high = GTK_LABEL(gtk_builder_get_object(builder, "lbl_high"));
    lbl_agc = GTK_LABEL(gtk_builder_get_object(builder, "lbl_agc"));
    lbl_if = GTK_LABEL(gtk_builder_get_object(builder, "lbl_if"));
    lbl_af = GTK_LABEL(gtk_builder_get_object(builder, "lbl_af"));

    btn_high = GTK_BUTTON(gtk_builder_get_object(builder, "btn_high"));
    btn_low = GTK_BUTTON(gtk_builder_get_object(builder, "btn_low"));
    btn_af = GTK_BUTTON(gtk_builder_get_object(builder, "btn_af"));
    btn_if = GTK_BUTTON(gtk_builder_get_object(builder, "btn_if"));
    btn_pitch = GTK_BUTTON(gtk_builder_get_object(builder, "btn_pitch"));
    btn_wpm = GTK_BUTTON(gtk_builder_get_object(builder, "btn_wpm"));
    btn_comp = GTK_BUTTON(gtk_builder_get_object(builder, "btn_comp"));
    btn_mic = GTK_BUTTON(gtk_builder_get_object(builder, "btn_mic"));
    btn_power = GTK_BUTTON(gtk_builder_get_object(builder, "btn_power"));
    btn_rx_tx = GTK_BUTTON(gtk_builder_get_object(builder, "btn_rx_tx"));

    ent_command = GTK_ENTRY(gtk_builder_get_object(builder, "ent_command"));

	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
								GTK_STYLE_PROVIDER(css_provider),
								GTK_STYLE_PROVIDER_PRIORITY_USER);
    
    highlight[se_af] = gtk_widget_get_style_context(GTK_WIDGET(lbl_af));
    highlight[se_high] = gtk_widget_get_style_context(GTK_WIDGET(lbl_high));
    highlight[se_low] = gtk_widget_get_style_context(GTK_WIDGET(lbl_low));
    highlight[se_if] = gtk_widget_get_style_context(GTK_WIDGET(lbl_if));
    highlight[se_pitch] = gtk_widget_get_style_context(GTK_WIDGET(lbl_pitch));
    highlight[se_wpm] = gtk_widget_get_style_context(GTK_WIDGET(lbl_wpm));
    highlight[se_comp] = gtk_widget_get_style_context(GTK_WIDGET(lbl_comp));
    highlight[se_mic] = gtk_widget_get_style_context(GTK_WIDGET(lbl_mic));
    highlight[se_power] = gtk_widget_get_style_context(GTK_WIDGET(lbl_power));


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

static void update_vfo_text(GtkLabel *label, char vfo_id, int frequency, Mode mode) {
    char temp[30];
    sprintf(temp, "VFO %c: %d %s", vfo_id, frequency, modes[mode]);
    gtk_label_set_text(label, temp);
}

void update_vfoa(int frequency, Mode mode) {update_vfo_text(lbl_vfoa, 'A', frequency, mode);}
void update_vfob(int frequency, Mode mode) {update_vfo_text(lbl_vfob, 'B', frequency, mode);}

void update_split(bool on) {gtk_label_set_text(lbl_split, off_on[on]);}
void update_rit(bool on) {gtk_label_set_text(lbl_rit, off_on[on]);}
void update_record(bool on) {gtk_label_set_text(lbl_record, off_on[on]);}

static void update_label_int(GtkLabel *label, int value) {
    char temp[20];
    sprintf(temp, "%d", value);
    gtk_label_set_text(label, temp);    
}

void update_power(int level) {update_label_int(lbl_power, level);}
void update_mic(int level) {update_label_int(lbl_mic, level);}
void update_comp(int level) {update_label_int(lbl_comp, level);}
void update_wpm(int wpm) {update_label_int(lbl_wpm, wpm);}
void update_pitch(int pitch) {update_label_int(lbl_pitch, pitch);}
void update_low(int frequency) {update_label_int(lbl_low, frequency);}
void update_high(int frequency) {update_label_int(lbl_high, frequency);}
void update_if(int level) {update_label_int(lbl_if, level);}
void update_af(int level) {update_label_int(lbl_af, level);}
void update_rx_tx(bool rx_tx) {gtk_button_set_label(btn_rx_tx, rx_txs[rx_tx]);}


void enable_highlight(SmEncoder item, bool on) {
    static void (* fn[])(GtkStyleContext *, const gchar *) = {
        gtk_style_context_remove_class,
        gtk_style_context_add_class
    };
    fn[on](highlight[item], "highlight");
} 


void btn_quit_clicked_cb(GtkButton *b) {
	gtk_main_quit();
}

void btn_minimize_clicked_cb(GtkButton *b) {
    gtk_window_iconify(GTK_WINDOW(window));
}


void btn_high_clicked_cb(GtkButton *b) {select_small_encoder(se_high);}
void btn_low_clicked_cb(GtkButton *b) {select_small_encoder(se_low);}
void btn_af_clicked_cb(GtkButton *b) {select_small_encoder(se_af);}
void btn_if_clicked_cb(GtkButton *b) {select_small_encoder(se_if);}
void btn_pitch_clicked_cb(GtkButton *b) {select_small_encoder(se_pitch);}
void btn_wpm_clicked_cb(GtkButton *b) {select_small_encoder(se_wpm);}
void btn_comp_clicked_cb(GtkButton *b) {select_small_encoder(se_comp);}
void btn_mic_clicked_cb(GtkButton *b) {select_small_encoder(se_mic);}
void btn_power_clicked_cb(GtkButton *b) {select_small_encoder(se_power);}

void btn_10m_clicked_cb(GtkButton *b) {do_10m();}
void btn_12m_clicked_cb(GtkButton *b) {do_12m();}
void btn_15m_clicked_cb(GtkButton *b) {do_15m();}
void btn_17m_clicked_cb(GtkButton *b) {do_17m();}
void btn_20m_clicked_cb(GtkButton *b) {do_20m();}
void btn_30m_clicked_cb(GtkButton *b) {do_30m();}
void btn_40m_clicked_cb(GtkButton *b) {do_40m();}
void btn_80m_clicked_cb(GtkButton *b) {do_80m();}

void btn_agc_clicked_cb(GtkButton *b) {do_agc();}
void btn_mode_clicked_cb(GtkButton *b) {do_mode();}
void btn_split_clicked_cb(GtkButton *b) {do_split();}
void btn_record_clicked_cb(GtkButton *b) {do_record();}
void btn_rx_tx_clicked_cb(GtkButton *b) {do_rx_tx();}
void btn_rit_clicked_cb(GtkButton *b) {do_rit();}
void btn_span_clicked_cb(GtkButton *b) {do_span();}
void btn_vfo_clicked_cb(GtkButton *b) {do_vfo();}
void btn_step_clicked_cb(GtkButton *b) {do_step();}