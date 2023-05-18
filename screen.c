#include "screen.h"
#include "lcd.h"
#include "portyLcd.h"
#include <msp430x14x.h>

#define LOFFSET 0
#define OFFSET 1


#define MUL_W 6
#define MUL_H 9
#define PIXEL_OFF 0x00
#define PIXEL_ON 0x01

unsigned long int lcd_data_presence;
unsigned char custom_char_counter;
unsigned long int line_info;

short iter = 0;

char screen[SCREEN_HEIGHT][SCREEN_WIDTH];


  


char pattern[8] = { 0x00 };
char lcd_pattern_line;


void send_custom_char(char *pattern)
{ 
  for ( int i = 0; i < 8; i++)
    SEND_CHAR(pattern[i]);
}

void hires_screen_show(short ball_x, short ball_y, short left_racket_h, short right_racket_h, short left_racket_width, short right_racket_width)
{ 
  if(ball_x > 0x7f){ ball_x = 41; P2OUT ^= BIT1; return;}
  if(ball_y > 0x7f){ ball_y = 8; P2OUT ^= BIT1; return;}
  
  
    
  lcd_data_presence = 0x0000; //pozioma obecnosc danych na ekranie - poczatkowo brak
  custom_char_counter = 0;   //zerowanie licznika custom charow


  //informacja dla ekranu o pikselach pilki i paletek
  screen[ball_y-2][ball_x-1] |= 0x01;
  screen[ball_y-2][ball_x] |= 0x01;
  screen[ball_y-2][ball_x+1] |= 0x01;
  screen[ball_y+2][ball_x-1] |= 0x01;
  screen[ball_y+2][ball_x] |= 0x01;
  screen[ball_y+2][ball_x+1] |= 0x01;
  screen[ball_y+1][ball_x-2] |= 0x01;
  screen[ball_y][ball_x-2] |= 0x01;
  screen[ball_y-1][ball_x-2] |= 0x01;
  screen[ball_y+1][ball_x+2] |= 0x01;
  screen[ball_y][ball_x+2] |= 0x01;
  screen[ball_y-1][ball_x+2] |= 0x01;


  screen[ball_y+1][ball_x-1] |= 0x01;
  screen[ball_y][ball_x] |= 0x01;
  screen[ball_y+1][ball_x] |= 0x01;
  screen[ball_y-1][ball_x+1] |= 0x01;
  screen[ball_y][ball_x+1] |= 0x01;
  screen[ball_y+1][ball_x+1] |= 0x01;
  
  //obecnosc pixeli pilki w danej kolumnie LCD
  lcd_data_presence |= (0x4000 >> (ball_x-2)/6);
  lcd_data_presence |= (0x4000 >> (ball_x)/6);
  lcd_data_presence |= (0x4000 >> (ball_x+2)/6);
  
  //informacja o rakietkach

  screen[left_racket_h][0+OFFSET] |= 0x01;
  screen[left_racket_h][1+OFFSET] |= 0x01;
  for(register short iter=1; iter<=left_racket_width; ++iter)
  {
    if(left_racket_h-iter>=0) //zeby nie pisalo nie swojej pamieci
    {
    screen[left_racket_h-iter][0+OFFSET] |= 0x01;
    screen[left_racket_h-iter][1+OFFSET] |= 0x01;
    }
    if(right_racket_h+iter<=16) //zeby nie pisalo nie swojej pamieci
    {
      screen[left_racket_h+iter][0+OFFSET] |= 0x01;
      screen[left_racket_h+iter][1+OFFSET] |= 0x01;
    }
  }

  screen[right_racket_h][SCREEN_WIDTH-LOFFSET-OFFSET-1] |= 0x01;
  screen[right_racket_h][SCREEN_WIDTH-LOFFSET-OFFSET-2] |= 0x01;
  

  for(register short iter=1; iter<=right_racket_width; ++iter)
  {
    screen[right_racket_h-iter][SCREEN_WIDTH-LOFFSET-OFFSET-1] |= 0x01;
    screen[right_racket_h+iter][SCREEN_WIDTH-LOFFSET-OFFSET-1] |= 0x01;
    screen[right_racket_h-iter][SCREEN_WIDTH-LOFFSET-OFFSET-2] |= 0x01;
    screen[right_racket_h+iter][SCREEN_WIDTH-LOFFSET-OFFSET-2] |= 0x01;
  }
   
  //obecnosc rakietek na kolumnach LCD
  lcd_data_presence |= 0x4000;
  lcd_data_presence |= 0x0002;
  
  
  lcd_pattern_line = 0x00;

  char i = 0x00;
  SEND_CMD(CLR_DISP);  
  SEND_CMD(CUR_HOME);
  
  line_info = 0x8000; //zmienna pomocnicza
  
  //magic :)
  for (i=0x00;i<0x0e;++i)
  {
    line_info >>= 1;
    if((lcd_data_presence & (line_info)) == line_info)
    {
      for(char h=0x00; h<0x08; ++h)
      {
        lcd_pattern_line = 0x00;
        for(char k=0; k<5; ++k)
        {
          lcd_pattern_line |= ( screen[h][k+6*i] << 4-k);
        }
        pattern[h] = lcd_pattern_line;  
      }      
    SEND_CMD(DD_RAM_ADDR+1+i); //0x80
    SEND_CHAR(0x00+custom_char_counter);
    SEND_CMD(0x40+8*custom_char_counter);
    ++custom_char_counter;
    send_custom_char(pattern);
    }
  }

  line_info = 0x8000;
  for (i=0x00;i<0x0e;++i)
  {
    line_info >>= 1;
    if((lcd_data_presence & line_info) == line_info)
    {
      int j = 0;
      for(char h=0x09; h<0x11; ++h)
      {
        lcd_pattern_line = 0x00;
        for(char k=0; k<5; ++k)
        {
          lcd_pattern_line |= ( screen[h][k+6*i] << 4-k);
        }
        pattern[j] = lcd_pattern_line;  
        ++j;
      }
    SEND_CMD(DD_RAM_ADDR2+1+i); //0x80
    SEND_CHAR(0x00+custom_char_counter);
    SEND_CMD(0x40+8*custom_char_counter);
    ++custom_char_counter;
    send_custom_char(pattern);
    }
  }
 
  
  // sprzatanie tablicy screen po pilkach i paletkch
  
  screen[ball_y-2][ball_x-1] = 0x00;
  screen[ball_y-2][ball_x] = 0x00;
  screen[ball_y-2][ball_x+1] = 0x00;
  screen[ball_y+2][ball_x-1] = 0x00;
  screen[ball_y+2][ball_x] = 0x00;
  screen[ball_y+2][ball_x+1] = 0x00;
  screen[ball_y+1][ball_x-2] = 0x00;
  screen[ball_y][ball_x-2] = 0x00;
  screen[ball_y-1][ball_x-2] = 0x00;
  screen[ball_y+1][ball_x+2] = 0x00;
  screen[ball_y][ball_x+2] = 0x00;
  screen[ball_y-1][ball_x+2] = 0x00;


  screen[ball_y+1][ball_x-1] = 0x00;
  screen[ball_y][ball_x] = 0x00;
  screen[ball_y+1][ball_x] = 0x00;
  screen[ball_y-1][ball_x+1] = 0x00;
  screen[ball_y][ball_x+1] = 0x00;
  screen[ball_y+1][ball_x+1] = 0x00;

  
  for(unsigned char i=0; i<SCREEN_HEIGHT; ++i)
  {
    screen[i][0+OFFSET] = 0x00;
    screen[i][1+OFFSET] = 0x00;
    screen[i][SCREEN_WIDTH-LOFFSET-OFFSET-1] = 0x00;
    screen[i][SCREEN_WIDTH-LOFFSET-OFFSET-2] = 0x00;
  }
}
