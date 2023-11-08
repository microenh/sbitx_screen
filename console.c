#include <ctype.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gmodule.h>
#include <string.h>

#include "console.h"
#include "display.h"
#include "radio_state.h"

const int CONSOLE_LINES = 21;


void update_console(const gchar *text) {
    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(tb_console, &iter);
    gtk_text_buffer_insert(tb_console, &iter, text, -1);
    gtk_text_buffer_insert(tb_console, &iter, "\r\n", 1);
    if (gtk_text_buffer_get_line_count(tb_console) > CONSOLE_LINES) {
        GtkTextIter iter1;
        gtk_text_buffer_get_start_iter(tb_console, &iter);
        gtk_text_buffer_get_iter_at_line(tb_console, &iter1, 1);
        gtk_text_buffer_delete(tb_console, &iter, &iter1);
    } 
}

void erase_console() {
    gtk_text_buffer_set_text(tb_console, "", -1);
}

void clear_console() {
    for (int i=0; i<CONSOLE_LINES; i++)
        update_console("");
}

/*
 * HELP
 * ----
 * audio
 * callsign [your callsign]
 * clear
 * cwinput
 * cw_tx_pitch
 * exchange
 * freq
 * ft8mode [auto|semiauto|manual]
 * grid [your grid]
 * l [callsign] [rst]
 * logbook
 * macro [macro name]
 * mode [CW|CWR|USB|LSB|RTTY|FT8|DIGITAL|2TONE]
 * qrz [callsign]
 * receive
 * sidetone
 * transmit
 * telnet [server]:[port]
 * tclose - close telnet session
 * txpitch [100-3000]
 * wpm
 * 
 * H2
 * --
 * s - view settings
 * mp - [blank|left|right|crosshair] - mouse pointer style
 * rs [on|off] - reverse scrolling
 * ta [on|off] - turns tuning acceleration on and off
 * tat1 [100-9999] - first threshold at which acceleration occurs (default 10,000)
 * tat2 [100-9999] - second threshold at which acceleration occurs (default 500)
 * cw
 * cwr
 * usb
 * lsb
 * rtty
 * ft8
 * digital (or dig)
 * 2tone 
 */


const gchar *help_text[] = {
    "HELP",
    "quit",
    "clear",
    "af",
    "if",
    "callsign [your callsign]",
    "grid [your grid]",
    "cwinput",
    "cw_tx_pitch",
    // "exchange",
    "freq",
    // "ft8mode [auto|semiauto|manual]",
    // "l [callsign] [rst]",
    // "logbook",
    // "macro [macro name]",
    "mode", // " [CW|CWR|USB|LSB|RTTY|FT8|DIGITAL|2TONE]",
    // "qrz [callsign]",
    // "receive",
    // "sidetone",
    // "transmit",
    // "telnet [server]:[port]",
    // "tclose - close telnet session",
    // "txpitch [100-3000]",
    "wpm",
    ""
};

// returns true and places result in *result if good integer text in input
// returns false and doesn't update *result if bad integer text in input
bool gstrtoi(int *result, gchar *input) {
    int r = 0;
    while (*input) {
        if (*input >= '0' && *input <= '9') {
            r = r * 10 + *input - '0';
        } else {
            return false;
        }
        input++;
    }
    *result = r;
    return true;
}

bool do_help(gchar *data) {
    // clear_console();
    erase_console();
    int i = 0;
    const char *l = help_text[i];
    while (*help_text[i]) {
        update_console(help_text[i++]);
    }
    return false;
}

bool do_quit(gchar *data) {
    gtk_main_quit();
    return false;
}

bool do_clear(gchar *data) {
    erase_console();
    return false;
}

bool do_freq(gchar *data) {
    int f;
    if (gstrtoi(&f, data)) {
        do_frequency(f);
        return true;
    }
    return false;
}

typedef bool (*CB_FUNC)(gchar *data);


typedef struct _dispatch {
    const gchar *command;
    CB_FUNC callback;
} DISPATCH;

static const DISPATCH dispatch[] = {
    {"H", do_help},
    {"Q", do_quit},
    {"C", do_clear},
    {"F", do_freq},
    {NULL, NULL}
};


void do_command(gchar *command, gchar *data) {
    int i;
    if (data && gstrtoi(&i, data)) {
        GString *text = g_string_new(NULL);
        g_string_printf(text, "%s: %d", command, i);
        update_console(text->str);
        g_string_free(text, true);    
    }
}

void parse_command(GString *command) {
    gchar *command_token;
    gchar *data_token = NULL;
    const char *s = " ";
    command_token = strtok(command->str, s);
    if (command_token) {
        data_token = strtok(NULL, s);
    }
    for (int i=0; dispatch[i].command; i++) {
        if (g_str_equal(command_token, dispatch[i].command)) {
            if (dispatch[i].callback(data_token)) {
                GString *text = g_string_new(NULL);
                g_string_printf(text, "%s: %d", command_token, data_token);
                update_console(text->str);
                g_string_free(text, true);    
            }
            break;
        }
    }
}

void do_console_entry() {
    GString *command = g_string_new(gtk_entry_get_text(ent_command));
    g_string_ascii_up(command);
    gtk_entry_set_text(ent_command, "");
    parse_command(command);
    g_string_free(command, true);
}