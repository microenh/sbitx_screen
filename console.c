#include <ctype.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <gmodule.h>
#include <string.h>

#include "console.h"
#include "display.h"
#include "global_string.h"
#include "hardware.h"
#include "radio_state.h"
#include "settings.h"

const int CONSOLE_LINES = 21;

void update_console(const gchar * const text) {
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


static const gchar * const help_text[] = {
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
bool gstrtoi(int *result, const gchar *input) {
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

const gchar * const upper_case(const gchar *input) {
    gchar *c = (gchar *)input;
    while (*c) {
        *c = toupper(*c);
        c++;
    }
    return input;
}

bool c_help(const gchar * const data) {
    // clear_console();
    erase_console();
    int i = 0;
    const char *l = help_text[i];
    while (*help_text[i]) {
        update_console(help_text[i++]);
    }
    return false;
}

bool c_quit(const gchar * const data) {
    gtk_main_quit();
    return false;
}

bool c_clear(const gchar * const data) {
    erase_console();
    return false;
}

bool c_freq(const gchar * const data) {
    int f;
    if (gstrtoi(&f, data)) {
        do_frequency(f);
        return true;
    }
    return false;
}


static const gchar * const bad_good[] = {
    "Bad ",
    ""
};

static void console_feedback(const gchar * const param, const gchar * const data, bool good) {
    g_string_printf(temp_string, "%s%s: %s", bad_good[good], param, data);
    update_console(temp_string->str);
}

bool c_call(const gchar * const data) {
    if (data)
        set_callsign(upper_case(data));
    console_feedback("Call", get_callsign(), true);
    return false;
}

bool c_grid(const gchar * const data) {
    if (data)
        set_grid(data);
    console_feedback("Grid", get_grid(), true);
    return false;
}

bool c_tx_lock(const gchar * const data) {
    if (data)
        set_tx_lock(strcmp(upper_case(data), off_on[0]));
    console_feedback("Tx Lock", off_on[get_tx_lock()], true);
    return false;
}

bool c_step(const gchar * const data) {
    if (data) {
        Step i;
        upper_case(data);
        for (i=0; i<s_END && strcmp(data, steps[i]); i++)
            ;
        bool good = i != s_END;
        if (good) 
            do_step(i);
        console_feedback("Step", data, good);
    }
    return false;
}

bool c_vfo(const gchar * const data) {
    if (data) {
        Vfo i;
        upper_case(data);
        for (i=0; i<v_END && strcmp(data, vfos[i]); i++)
            ;
        bool good = i != v_END;
        if (good) 
            do_vfo(i);
        console_feedback("VFO", data, good);
    }
    return false;
}

bool c_span(const gchar * const data) {
    if (data) {
        Span i;
        upper_case(data);
        for (i=0; i<sp_END && strcmp(data, spans[i]); i++)
            ;
        bool good = i != sp_END;
        if (good) 
            do_span(i);
        console_feedback("Span", data, good);
    }
    return false;
}

bool c_rit(const gchar * const data) {
    if (data) {
        upper_case(data);
        bool b = (!strcmp(data, off_on[1]));
        do_rit(b);
        console_feedback("RIT", off_on[b], true);
    }
    return false;
}

bool c_split(const gchar * const data) {
    if (data) {
        upper_case(data);
        bool b = (!strcmp(data, off_on[1]));
        do_split(b);
        console_feedback("Split", off_on[b], true);
    }
    return false;
}

bool c_record(const gchar * const data) {
    if (data) {
        upper_case(data);
        bool b = (!strcmp(data, off_on[1]));
        do_record(b);
        console_feedback("Record", off_on[b], true);
    }
    return false;
}

bool c_agc(const gchar * const data) {
    if (data) {
        Agc i;
        upper_case(data);
        for (i=0; i<a_END && strcmp(data, agcs[i]); i++)
            ;
        bool good = i != a_END;
        if (good) 
            do_agc(i);
        console_feedback("AGC", data, good);
    }
    return false;
}

static bool c_subEncoder(const SubEncoder rse, const gchar * const data) {
    int i;
    if (gstrtoi(&i, data)) {
        do_sub_encoder(rse, i);
        return false;
    }
    return true;
}

static bool c_filter(const gchar * const data) {
    if (data) {
        int pin;
        upper_case(data);
        hw_set_filter(data[0]);
    }
}

// bool c_af(const gchar * const data) {return c_subEncoder(se_af, data);}
// bool c_comp(const gchar * const data) {return c_subEncoder(se_comp, data);}
// bool c_high(const gchar * const data) {return c_subEncoder(se_high, data);}
// bool c_if(const gchar * const data) {return c_subEncoder(se_if, data);}
// bool c_low(const gchar * const data) {return c_subEncoder(se_low, data);}
// bool c_mic(const gchar * const data) {return c_subEncoder(se_mic, data);}
// bool c_pitch(const gchar * const data) {return c_subEncoder(se_pitch, data);}
// bool c_power(const gchar * const data) {return c_subEncoder(se_power, data);}
// bool c_wpm(const gchar * const data) {return c_subEncoder(se_wpm, data);}


struct _dispatch {
    const gchar * const command;
    bool (*callback)(gchar const * const data);
};

static const struct _dispatch const dispatch[] = {
    {"H", c_help},
    {"Q", c_quit},
    {"C", c_clear},
    {"F", c_freq},
    {"CALL", c_call},
    {"GRID", c_grid},
    {"LOCK", c_tx_lock},
    {"STEP", c_step},
    {"VFO", c_vfo},
    {"SPAN", c_span},
    {"RIT", c_rit},
    {"SPLIT", c_split},
    {"REC", c_record},
    {"AGC", c_agc},
    {"FILTER", c_filter},
    // {"AF", c_af},
    // {"COMP", c_comp}.
    // {"HIGH", c_high},
    // {"IF", c_if},
    // {"LOW", c_low},
    // {"MIC", c_mic},
    // {"PITCH", c_pitch},
    // {"POWER", c_power},
    // {"WPM", c_wpm},
    {NULL, NULL}
};

void parse_command(const GString * const command) {
    gchar *command_token;
    gchar *data_token = NULL;
    const char *s = " ";
    command_token = strtok(command->str, s);
    if (command_token) {
        data_token = strtok(NULL, s);
        upper_case(command_token);
    }
    // check for 10M .. 80M
    for (Band b=0; b<b_END; b++) {
        if (g_str_equal(command_token, bands[b])) {
            do_band(b);
            console_feedback("Band", command_token, true);
            return;   
        }
    }
    // check for mode (LSB, USB, ...)
    for (Mode m=0; m<m_END; m++) {
        if (g_str_equal(command_token, modes[m])) {
            do_mode(m);
            console_feedback("Mode", command_token, true);
            return;
        }
    }
    // check for sub-encoder settings
    for (SubEncoder s=0; s<se_END; s++) {
        if (g_str_equal(command_token, subEncoders[s])) {
            c_subEncoder(s, data_token);        
        }
    }
    for (int i=0; dispatch[i].command; i++) {
        if (g_str_equal(command_token, dispatch[i].command)) {
            if (dispatch[i].callback(data_token)) {
                g_string_printf(temp_string, "%s: %s", command_token, data_token);
                update_console(temp_string->str);
            }
            break;
        }
    }
}

void do_console_entry() {
    g_string_assign(temp_string, gtk_entry_get_text(ent_command));
    gtk_entry_set_text(ent_command, "");
    parse_command(temp_string);
}