/********************************************
*
*  Name: Alec Fong
*  Section:
*  Assignment: Lab 11 - AC UNIT FROM HELL
*
********************************************/

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "lcd.h"
#include "LED.h"
#include "ds1631.h"
#include "push_buttons.h"

#define FOSC 16000000 //clock freq
#define BAUD 9600 //baud rate used
#define UBRR FOSC/16/BAUD-1 //value for UBRR0




void display_top(unsigned char temp,char* rmt);
void display_bot(int low, int high);
void display(unsigned char temp,char* rmt, int low, int high);

char high_low = 0;
int high = 80;
int low = 60;
volatile char update = 0;
char stateA = 0;
char stateB = 0;
char state0;
char state1;
char state2;
char state3;
char last_state = 0;
void init_encoder();

unsigned char get_fahrenheit(unsigned char*t);
void init_temp_sensor(void);

volatile char flag;
volatile char rec_data[4];//buffer
char char_count = 0;
void transmit_temp(unsigned char temp);
void tChar_rs232(unsigned char x);
void init_rs232();



int main(void) {
  init_rs232();

  init_leds();

  init_encoder();
  
  init_push_buttons();

  init_temp_sensor();
  unsigned char temp[2];//temp data char
  unsigned char old_temp = 0;
  unsigned char cur_temp;

  init_lcd();
  writecommand(0x01);//clear screen
  char c[40];
  char rmt[16];
  rmt[0] = '0';
  rmt[1] = '0';
  rmt[2] = '\0';

  //SPLASH SCREEN
  cur_temp = get_fahrenheit(temp);
  get_cstring(cur_temp, c);
  writecommand(0x01);//clear screen
  stringout("Alec's AC Unit/nTemp: ");
  stringout(c);
  _delay_ms(2000);

  display(cur_temp, rmt, low,high);
  
  

  while (1) {// Loop forever

    cur_temp = get_fahrenheit(temp);

    //IF TEMP CHANGE -> UPDATE SCREEN
    if (cur_temp != old_temp)
    {
      display(cur_temp, rmt, low,high);
      old_temp = cur_temp;

      transmit_temp(cur_temp);
    }

    //IF FINISHED RECIEVING TEMP
    if (flag == 1)
    {
      if (rec_data[1] == '0')
      {
        rmt[0] = rec_data[2];
        rmt[1] = rec_data[3];
        rmt[2] = '\0';
      }
      else
      {
        rmt[0] = rec_data[1];
        rmt[1] = rec_data[2];
        rmt[2] = rec_data[3];
        rmt[3] = '\0';
      }
      display(cur_temp, rmt, low,high);
    }

    //CHECK IF BUTTON IS PRESSED
    if (is_red())
    {
      high_low = 1;
    }
    else if (is_blue())
    {
      high_low = 0;
    }

    //CHECK IF ROTARY ENCODER IS TURNED
    if (update)
    {
      display(cur_temp, rmt, low,high);
    }

    //TURN ON "AC"(LEDS) ACCORDING TO HIGH, LOW, AND CURRENT TEMP
    if(high < cur_temp){
      set_green_on();
    }
    else{
      set_green_off();
    } 
    if(low > cur_temp){
      set_red_on();
    } 
    else{
      set_red_off();
    }
        
  }
  return 0;   /* never reached */
}



void init_rs232()
{
  UCSR0B |= (1 << TXEN0 | 1 << RXEN0 | 1 << RXCIE0);
  UCSR0C = (3 << UCSZ00);
  DDRD |= (1 << PD1); // initialize PD1 for TX output
  UBRR0 = UBRR; //set baud rate

  DDRC |= (1<<PC1); //set A1 to output (used for tristate power)
  PORTC &= ~(1<<PC1); //set A1 to 0
}

void tChar_rs232(unsigned char x)//WILL TRANSMIT CHAR X
{
  while((UCSR0A & (1 << UDRE0)) == 0){}
  UDR0 = x;
}

void transmit_temp(unsigned char temp)
{
  char tr[5];
  if (temp < 100)
  {
    snprintf(tr,5,"+0%d",temp);
  }
  else
  {
    snprintf(tr,5,"+%d",temp);
  }
  tChar_rs232(tr[0]);
  tChar_rs232(tr[1]);
  tChar_rs232(tr[2]);
  tChar_rs232(tr[3]);
}

ISR(USART_RX_vect)
{
  unsigned char rec = UDR0;
  if (rec == '-' || rec == '+')
  {
    rec_data[0] = rec;
    char_count = 1;
    flag = 0;
  }
  else if (char_count == 1)
  {
    rec_data[1] = rec;
    char_count++;
    flag = 0;
  }
  else if (char_count == 2)
  {
    rec_data[2] = rec;
    char_count++;
    flag = 0;
  }
  else if (char_count == 3)
  {
    rec_data[3] = rec;
    char_count++;
    flag = 1;
  }
}


void init_temp_sensor()
{
  ds1631_init();//init temp sensor
  ds1631_conv();// Set the temp sensor to do conversions
}

unsigned char get_fahrenheit(unsigned char*t)
{
  ds1631_temp(t);

  unsigned char temp = t[0];
  temp *= 2;
  if (t[1] != 0)
  {
    temp += 1;
  }
  temp = (temp*9)/10;
  temp += 32;
  return temp;
}


void init_encoder()
{
  PCICR |= (1<<PCIE1);//init interrupt
  PCMSK1 |= (1<<PCINT10)|(1<<PCINT11);//set interupts for pin a3 and a2
  PORTC |= (1 << PC2) | (1 << PC3); //pull up resistors for motor encoder
  sei(); //allow global interupts
}


ISR(PCINT1_vect){
  
  stateB = ((PINC & (1<<PC2))>>PC2);
  stateA = ((PINC & (1<<PC3))>>PC3);
  state0 = (stateA == 0 && stateB == 0);
  state1 = (stateA == 0 && stateB != 0);
  state2 = (stateA != 0 && stateB != 0);
  state3 = (stateA != 0 && stateB == 0);
  //IF HIGH IS SET
  if(high_low)
  { 
    //ENSURING HIGH IS ALWAYS GREATER THAN LOW
    if(high > low) 
    {
        if (state0)
        {
          if (last_state == 1)
          {
            high--;
          }
          else if (last_state == 3)
          {
            high++;
          }
          last_state = 0;
        }
        else if (state1)
        {
          if (last_state == 2)
          {
            high--;
          }
          else if (last_state == 0)
          {
            high++;
          }
          last_state = 1;
        }
        else if (state2)
        {
          if (last_state == 3)
          {
            high--;
          }
          else if (last_state == 1)
          {
            high++;
          }
          last_state = 2;
        }
        else if (state3)
        {
          if (last_state == 0)
          {
            high--;
          }
          else if (last_state == 2)
          {
            high++;
          }
          last_state = 3;
        }
        update = 1;
    }
    //FIX CASES WHERE HIGH IS LESS THAN OR EQUAL TO LOW
    if (high <= low)
        {
          high = low+1;
        }    
  }
  else //IF LOW IS SET
  {
    //ENSURING LOW IS ALWAYS LESS THAN HIGH
    if (low < high)
    {
      if (state0)
        {
          if (last_state == 1)
          {
            low--;
          }
          else if (last_state == 3)
          {
            low++;
          }
          last_state = 0;
        }
        else if (state1)
        {
          if (last_state == 2)
          {
            low--;
          }
          else if (last_state == 0)
          {
            low++;
          }
          last_state = 1;
        }
        else if (state2)
        {
          if (last_state == 3)
          {
            low--;
          }
          else if (last_state == 1)
          {
            low++;
          }
          last_state = 2;
        }
        else if (state3)
        {
          if (last_state == 0)
          {
            low--;
          }
          else if (last_state == 2)
          {
            low++;
          }
          last_state = 3;
        }
        update = 1;
      if (low >= high)
      {
        low = high-1;
      }  
    }
  }
}


void display(unsigned char temp,char* rmt, int low, int high)
{
  moveto(0x00);
  char disp1[40];
  snprintf(disp1,40,"Temp: %d Rmt:",temp);
  stringout(disp1);
  stringout(rmt);
  moveto(0x40);
  char disp2[40];
  snprintf(disp2,40,"Low:%d High:%d",low,high);
  stringout(disp2); 
}

void display_top(unsigned char temp,char* rmt)
{
  moveto(0x00);
  char disp1[40];
  snprintf(disp1,40,"Temp: %d Rmt:",temp);
  stringout(disp1);
  stringout(rmt);
}

void display_bot(int low, int high)
{
  moveto(0x40);
  char disp2[40];
  snprintf(disp2,40,"Low:%d High:%d",low,high);
  stringout(disp2); 
}

// void init_push_buttons()
// {
//   DDRB &= ~(1 << PB3) | (1 << PB4);//clear and set as input
//   PORTB |= (1 << PB3) | (1 << PB4); //pull up resistors for push buttons
// }
// char is_blue()
// {
//   if ((PINB & (1<<PB4))==0)//BLUE
//   {
//     return 1;
//   }
//   else 
//     return 0;
// }
// char is_red()
// {
//   if ((PINB & (1<<PB3))==0)//RED
//   {
//     return 1;
//   }
//   else 
//     return 0;
// }

// void init_leds()
// {
//   DDRD |= (1<<DD3) | (1<<DD2); //led set as outputs
// }
// void set_red_on()
// {
//   PORTD |= (1 << PD2);//red on
// }
// void set_red_off()
// {
//   PORTD &= ~(1 << PD2);//red off
// }
// void set_green_on()
// {
//   PORTD |= (1 << PD3);//green on
// }
// void set_green_off()
// {
//   PORTD &= ~(1 << PD3);//green off
// }
