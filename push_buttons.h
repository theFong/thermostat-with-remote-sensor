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

char is_red(void);
char is_blue(void);
void init_push_buttons(void);
