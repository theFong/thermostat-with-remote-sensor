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
#include "lcd.h"

void writenibble(unsigned char);

unsigned char cursor = 0x00;

void init_lcd()
{
    DDRD &= ~((1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7));//clear register D
    DDRD |= ((1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7));//sets D4-D7 as output; used for sending bits
    DDRB &= ~((1<<PB0)|(1<<PB1));//clear register B
    DDRB |= ((1<<PB0)|(1<<PB1));//sets D8 & D9 as output; PB0/D8 used for Register Select(RS), PB1/D9 used for Enable
    _delay_ms(15);
    PORTB &= ~(1<<PB0);              // Delay at least 15ms
    writenibble(0b00110000);
    // Use writenibble to send 0011
    _delay_ms(5);               // Delay at least 4msec
    writenibble(0b00110000);
    // Use writenibble to send 0011
    _delay_us(100);             // Delay at least 100usec
    writenibble(0b00110000);
    // Use writenibble to send 0011, no delay needed
    writenibble(0b00100000);
    // Use writenibble to send 0010    // Function Set: 4-bit interface
    _delay_ms(2);
    
    writecommand(0x28);         // Function Set: 4-bit interface, 2 lines

    writecommand(0x0f);         // Display and cursor on
    _delay_ms(2);

}

/*
  stringout - Print the contents of the character string "str"
  at the current cursor position.
*/
void stringout(char *str)
{
  char last_char;
  while(*str != '\0')
  {
    if (!((last_char == '/' && *str == 'n') || (*str == '/' && *(str+1) == 'n')))//if not write data
    {
      writedata(*str);
    }
    last_char = *str;
    cursor++;
    str++;
    if ((cursor > 0x0f) && (cursor < 0x40) || (last_char == '/' && *str == 'n'))//allows for texts wraps and if /n is typed
    {
      cursor =  0x40;
      moveto(0x40);

    }
  }
}

void ustringout(unsigned char *str)
{
  char last_char;
  while(*str != '\0')
  {
    if (!((last_char == '/' && *str == 'n') || (*str == '/' && *(str+1) == 'n')))//if not write data
    {
      writedata(*str);
    }
    last_char = *str;
    cursor++;
    str++;
    if ((cursor > 0x0f) && (cursor < 0x40) || (last_char == '/' && *str == 'n'))//allows for texts wraps and if /n is typed
    {
      cursor =  0x40;
      moveto(0x40);

    }
  }
}

/*
  moveto - Move the cursor to the postion "pos"
*/
void moveto(unsigned char pos)
{
  cursor = pos;
  writecommand((0x80+pos));
}

/*
  writecommand - Output a byte to the LCD display instruction register.
*/
void writecommand(unsigned char x)
{
  PORTB &= ~(1<<PB0);//sets RS as 0 for command
  writenibble(x);
  writenibble(x<<4);
  _delay_ms(2);
}

/*
  writedata - Output a byte to the LCD display data register
*/
void writedata(unsigned char x)
{
  PORTB |= (1<<PB0);//sets RS as 1 for data
  writenibble(x);
  writenibble(x<<4);
  _delay_ms(2);
}

/*
  writenibble - Output four bits from "x" to the display
*/
void writenibble(unsigned char x)
{
  PORTD &= ~((1<<PD4)|(1<<PD5)|(1<<PD6)|(1<<PD7));//clear D4-D7
  PORTD |= (x & (0xf0));//copies most significant bits to PORTD
  //enable 125nsec twice
  PORTB |= (1<<PB1);
  PORTB |= (1<<PB1);
  PORTB &= ~(1<<PB1);

}
