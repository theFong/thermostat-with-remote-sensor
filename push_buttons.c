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
#include "push_buttons.h"
#include <avr/interrupt.h>

void init_push_buttons()
{
  DDRB &= ~(1 << PB3) | (1 << PB4);//clear and set as input
  PORTB |= (1 << PB3) | (1 << PB4); //pull up resistors for push buttons
}
char is_blue()
{
  if ((PINB & (1<<PB4))==0)//BLUE
  {
    return 1;
  }
  else 
    return 0;
}
char is_red()
{
  if ((PINB & (1<<PB3))==0)//RED
  {
    return 1;
  }
  else 
    return 0;
}