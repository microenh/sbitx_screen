
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "tmate2.h"

#include "radio_state.h"
#include "tmate2/tmate2.h"

void cb_main_move(int delta) {
    do_main_encoder(delta);
}

void cb_f6_tap(void) {
    gtk_main_quit();
}

void tmate2_init(void) {
	if (!init_tmate2()) {
		puts("Can't initialize TMate2");
		exit(EXIT_FAILURE);
	}
    set_rgb(0xff, 0xff, 0xff);
    set_main_move(cb_main_move);
    set_f6_tap(cb_f6_tap);
}


void tmate2_close(void) {
    set_rgb(0,0,0);
    close_tmate2();
}