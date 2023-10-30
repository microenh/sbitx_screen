#include <stdbool.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>

#include "display.h"
#include "radio.h"

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
    *btn_power;


static GtkEntry* ent_command;

static GtkWidget *window;


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

    ent_command = GTK_ENTRY(gtk_builder_get_object(builder, "ent_command"));

	gtk_style_context_add_provider_for_screen(gdk_screen_get_default(),
								GTK_STYLE_PROVIDER(css_provider),
								GTK_STYLE_PROVIDER_PRIORITY_USER);
	
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);


	gtk_widget_show(window);
	gtk_window_fullscreen(GTK_WINDOW(window));
}


void update_console(char *text) {gtk_label_set_text(lbl_console, text);}
void update_step(char *text) {gtk_label_set_text(lbl_step, text);}
void update_vfo(char *text) {gtk_label_set_text(lbl_vfo, text);}
void update_span(char *text) {gtk_label_set_text(lbl_span, text);}
void update_rit(char *text) {gtk_label_set_text(lbl_rit, text);}
void update_vfob(char *text) {gtk_label_set_text(lbl_vfob, text);}
void update_vfoa(char *text) {gtk_label_set_text(lbl_vfoa, text);}
void update_date(char *text) {gtk_label_set_text(lbl_date, text);}
void update_power(char *text) {gtk_label_set_text(lbl_power, text);}
void update_record(char *text) {gtk_label_set_text(lbl_record, text);}
void update_mic(char *text) {gtk_label_set_text(lbl_mic, text);}
void update_comp(char *text) {gtk_label_set_text(lbl_comp, text);}
void update_wpm(char *text) {gtk_label_set_text(lbl_wpm, text);}
void update_pitch(char *text) {gtk_label_set_text(lbl_pitch, text);}
void update_split(char *text) {gtk_label_set_text(lbl_split, text);}
void update_mode(char *text) {gtk_label_set_text(lbl_mode, text);}
void update_low(char *text) {gtk_label_set_text(lbl_low, text);}
void update_high(char *text) {gtk_label_set_text(lbl_high, text);}
void update_agc(char *text) {gtk_label_set_text(lbl_agc, text);}
void update_if(char *text) {gtk_label_set_text(lbl_if, text);}
void update_af(char *text) {gtk_label_set_text(lbl_af, text);}

void enable_high(bool enable) {gtk_widget_set_sensitive(GTK_WIDGET(btn_high), enable);}
void enable_low(bool enable) {gtk_widget_set_sensitive(GTK_WIDGET(btn_low), enable);}
void enable_af(bool enable) {gtk_widget_set_sensitive(GTK_WIDGET(btn_af), enable);}
void enable_if(bool enable) {gtk_widget_set_sensitive(GTK_WIDGET(btn_if), enable);}
void enable_pitch(bool enable) {gtk_widget_set_sensitive(GTK_WIDGET(btn_pitch), enable);}
void enable_wpm(bool enable) {gtk_widget_set_sensitive(GTK_WIDGET(btn_wpm), enable);}
void enable_comp(bool enable) {gtk_widget_set_sensitive(GTK_WIDGET(btn_comp), enable);}
void enable_mic(bool enable) {gtk_widget_set_sensitive(GTK_WIDGET(btn_mic), enable);}
void enable_power(bool enable) {gtk_widget_set_sensitive(GTK_WIDGET(btn_power), enable);}

void btn_quit_clicked_cb(GtkButton *b) {
	gtk_main_quit();
}

void btn_minimize_clicked_cb(GtkButton *b) {
    gtk_window_iconify(GTK_WINDOW(window));
}


void btn_high_clicked_cb(GtkButton *b) {do_high();}
void btn_low_clicked_cb(GtkButton *b) {do_low();}
void btn_af_clicked_cb(GtkButton *b) {do_af();}
void btn_if_clicked_cb(GtkButton *b) {do_if();}
void btn_pitch_clicked_cb(GtkButton *b) {do_pitch();}
void btn_wpm_clicked_cb(GtkButton *b) {do_wpm();}
void btn_comp_clicked_cb(GtkButton *b) {do_comp();}
void btn_mic_clicked_cb(GtkButton *b) {do_mic();}
void btn_power_clicked_cb(GtkButton *b) {do_power();}

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
void btn_rx_clicked_cb(GtkButton *b) {do_rx();}
void btn_tx_clicked_cb(GtkButton *b) {do_tx();}
void btn_rit_clicked_cb(GtkButton *b) {do_rit();}
void btn_span_clicked_cb(GtkButton *b) {do_span();}
void btn_vfo_clicked_cb(GtkButton *b) {do_vfo();}
void btn_step_clicked_cb(GtkButton *b) {do_step();}