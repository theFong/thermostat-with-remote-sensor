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

void init_lcd(void);
void stringout(char *);
void moveto(unsigned char);

void writecommand(unsigned char);
void writedata(unsigned char);
