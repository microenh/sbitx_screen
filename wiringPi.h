#pragma once

#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1
#define PUD_UP 1
#define INT_EDGE_BOTH 2

void digitalWrite(int pin, int state);
int digitalRead(int pin);
void pinMode(int pin, int mode);
void wiringPiSetup(void);
void pullUpDnControl(int pin, int mode);
void wiringPiISR(int pin, int trigger, void (*callback)(void));
void delay(int period);