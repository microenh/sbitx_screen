#include "wiringPi.h"

void digitalWrite(int pin, int state) {}

int digitalRead(int pin) {return 1;}
void pinMode(int pin, int mode) {}
void wiringPiSetup(void) {}
void pullUpDnControl(int pin, int mode) {}
void wiringPiISR(int pin, int trigger, void (*callback)(void)) {}
void delay(int period){}
