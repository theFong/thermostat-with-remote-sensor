/********************************************
*
*  Name: Alec Fong
*  Section:
*  Assignment: Lab 9 - lcd.h
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>

void set_green_off(void);
void set_green_on(void);
void set_red_off(void);
void set_red_on(void);
void init_leds(void);
