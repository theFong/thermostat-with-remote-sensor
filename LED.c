/********************************************
*
*  Name: Alec Fong
*  Section:
*  Assignment: 
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "LED.h"
#include <avr/interrupt.h>

void init_leds()
{
  DDRD |= (1<<DD3) | (1<<DD2); //led set as outputs
}
void set_red_on()
{
  PORTD |= (1 << PD2);//red on
}
void set_red_off()
{
  PORTD &= ~(1 << PD2);//red off
}
void set_green_on()
{
  PORTD |= (1 << PD3);//green on
}
void set_green_off()
{
  PORTD &= ~(1 << PD3);//green off
}