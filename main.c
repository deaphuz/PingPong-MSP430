#include<msp430x14x.h>
#include "lcd.h"
#include "screen.h"
#include "text.h"
#include "portyLcd.h"
#include "scroll.h"
#define BALL_START_X 41
#define BALL_START_Y 8
#define GAME_SPEED 60
#define ANY_BUTTON_CLICKED !((P4IN & BIT4)==BIT4) || !((P4IN & BIT5)==BIT5) || !((P4IN & BIT6)==BIT6)  ||  !((P4IN & BIT7)==BIT7)

short left_results[5] = {0, 0, 0, 0, 0};
short right_results[5] = {0, 0, 0, 0, 0};
float abs(float value)
{
  if(value < 0)
    return value*(-1);
  else return value;
}
void menu(void);
void game(void);
void highscores(void);
void end_game(short left_points, short right_points);
void main( void )
{
  P1SEL &= ~BIT5; //IO na przekaznik 1
  P1DIR |= BIT5;  //bit5 jako wyjscie
  P1SEL &= ~BIT6; //IO na przekaznik 2
  P1DIR |= BIT6;  //bit6 jako wyjscie
  P2DIR |= BIT1;  // STATUS LED
  // na detekcje klawiszy
  P4DIR &= ~BIT4;          
  P4DIR &= ~BIT5;        
  P4DIR &= ~BIT6;        
  P4DIR &= ~BIT7;        
  P1OUT &= ~BIT5;
  P1OUT &= ~BIT6;
  WDTCTL=WDTPW+ WDTHOLD;      // zatrzymanie WDT
  InitPortsLcd();                        // inicjalizacja portów  
  InitLCD();                             // inicjalizacja LCD
  clearDisplay();                        // czyszczenie LCD  
  BCSCTL1 |= XTS;                 // ACLK = LFXT1 = HF XTAL 8MHz
  BCSCTL1 |= DIVA_0;                    // ACLK=8 MHz
  BCSCTL2 |= SELM0 | SELM1;             // MCLK= LFTX1 =ACLK
  Delayx100us(20);
  menu();
}

void menu(void)
{
  clearDisplay();
  while(1)
  {
    Delayx100us(1);
    SEND_CMD(DD_RAM_ADDR);
    writeText("1.Graj", 6);
    SEND_CMD(DD_RAM_ADDR+8);
    writeText("2.Wyniki", 8);
    SEND_CMD(DD_RAM_ADDR2);
    writeText("3.Opis", 6);
    SEND_CMD(DD_RAM_ADDR2+7);
    writeText("4.Autorzy", 9);
  	if(!((P4IN & BIT4)==BIT4))        //detekcja guzika B1
  	{ 
    		game();
                clearDisplay();
  	}
  	if(!((P4IN & BIT5)==BIT5))        //detekcja guzika B2
  	{

             highscores();
             clearDisplay();
           
  	}
  	if(!((P4IN & BIT6)==BIT6))        //detekcja guzika B3
  	{
           description();
           clearDisplay();

  	}
  	if(!((P4IN & BIT7)==BIT7))        //detekcja guzika B4
  	{  
           authors();
           clearDisplay();

  	}
  }
}


void highscores(void)
{
  Delayx100us(200);
  for(int i=0; i<=3; ++i)
  {
    clearDisplay();
    Delayx100us(2);
    SEND_CMD(DD_RAM_ADDR);
    Delayx100us(1);
    for(int j=0; j<2; ++j)
    {
      if(j==0){ SEND_CHAR(48+i);}
      if(i<4 && j==1)
      {
        SEND_CMD(DD_RAM_ADDR2);
         SEND_CHAR(48+i+1);
      }    
      SEND_CHAR('.');
      SEND_CHAR(' ');
      SEND_CHAR('L');
      SEND_CHAR(':');
      SEND_CHAR(' ');
      if(left_results[i+j] == 10)
      {
        SEND_CHAR('1');
        SEND_CHAR('0');
      }
      else
        SEND_CHAR(48+left_results[i+j]);
      SEND_CHAR(',');
      SEND_CHAR(' ');
      
      SEND_CHAR('R');
      SEND_CHAR(':');
      SEND_CHAR(' ');
      if(right_results[i+j] == 10)
      {
        SEND_CHAR('1');
        SEND_CHAR('0');
      }
      else
        SEND_CHAR(48+right_results[i+j]);
    }
    while( !( ANY_BUTTON_CLICKED ) );
    Delayx100us(100);
  }
  Delayx100us(200);
  return;
}


  
void game(void)
{
  signed short game_speed = 0;
  signed short ball_x, left_racket_h, right_racket_h = 0;
  float ball_y = 0;
  signed short left_racket_width = 4;
  signed short right_racket_width = 4;
  short next = 0;
  unsigned short left_points = 0;
  unsigned short right_points = 0;
  
  //----------------------------------------------------------------
  
  //ustawienia poczatkowe gry - nie zmieniac!
  ball_x = BALL_START_X;
  ball_y = BALL_START_Y;

  left_racket_h = 8;
  right_racket_h = 8;
  //-----------------------------------------
  
  //nad tym mozna sie zastanowic...
  game_speed = GAME_SPEED;

  int xdir = 1;
  int ydir = 1;
  
       ball_x = BALL_START_X;
       ball_y = BALL_START_Y;
       next = 0;
       game_speed = GAME_SPEED;
       right_racket_width = 4;
       left_racket_width = 4;
       left_racket_h = 8;
       right_racket_h = 8;
  
  while (1)           // nieskoñczona pêtla 
  {
    if(next)
    {
       ball_x = BALL_START_X;
       ball_y = BALL_START_Y;
       next = 0;
       game_speed = GAME_SPEED;
       right_racket_width = 4;
       left_racket_width = 4;
       if(left_points>9 || right_points>9)
       {
	 end_game(left_points, right_points);
         menu();
       }
       left_racket_h = 8;
       right_racket_h = 8;
       
    }
    
    // limity na paletki zeby nie wypisywaly glupot
    // w pamieci bedacej poza plansza
    if(left_racket_h<0)
      left_racket_h = 0;
    if(left_racket_h>16)
      left_racket_h = 16;
    if(right_racket_h<0)
      right_racket_h = 0;
    if(right_racket_h>16)
      right_racket_h = 16;

   //detekcja kolizji gora/dol ekranu + SFX
   if(ball_y<=0)
   {
     ydir=1;
     P1OUT |= BIT5;
   }
   if(ball_y>=16)
   {
     ydir=0;
     P1OUT |= BIT6;
   }

   if(ball_x<=5)
   {
     if(abs((ball_y-left_racket_h)) >= (2 + left_racket_width) )
     {
       next = 1;
       ++right_points;  

     }
     else
     {
       xdir = 1;
       P1OUT |= BIT5;
       game_speed+=5;
       if(game_speed%20==0 && right_racket_width>1)
  	 {
     		--right_racket_width;
    		 --left_racket_width;
  	 }
     }
     
   }

   if(ball_x>=77)
   {
     if(abs((ball_y-right_racket_h)) >= (2 + right_racket_width) )
     {
       next = 1;
       ++left_points;
     }
     else
     {
       xdir = 0;
       P1OUT |= BIT6;
       game_speed+=5;
       if(game_speed%20==0 && right_racket_width>1)
  	 {
     		--right_racket_width;
    		 --left_racket_width;
  	 }
     }
   }
   if(game_speed>127) //fix na nie przekrecanie zmiennej game_speed
     game_speed=127;
   

   
   // poruszanie sie po osi x - nie dotykac
   if(xdir) ball_x+=3;
   else ball_x-=3;
   
   //poruszanie po osi y - dotykac
   if(ydir) ++ball_y;
   else --ball_y;
     

  if(!((P4IN & BIT4)==BIT4))        //detekcja guzika B1
  { 
    if(left_racket_h>0)
      --left_racket_h;   //lewa rakietka w gore
  }
  if(!((P4IN & BIT5)==BIT5))        //detekcja guzika B2
  {  
    if(left_racket_h<16)
      ++left_racket_h;   //lewa rakietka w dol
  }
  if(!((P4IN & BIT6)==BIT6))        //detekcja guzika B3
  {
    if(right_racket_h>0)
      --right_racket_h;  //prawa rakietka w gore
  }
  if(!((P4IN & BIT7)==BIT7))        //detekcja guzika B4
  {  
    if(right_racket_h<16)
      ++right_racket_h;  //prawa rakietka w dol
  }
  
  //wyswietlanie tresci wysokiej rozdzielczosci na ekranie

  hires_screen_show(ball_x, (short)ball_y, left_racket_h, right_racket_h, left_racket_width, right_racket_width);
    SEND_CMD(DD_RAM_ADDR);
  SEND_CHAR(48+left_points);
  SEND_CMD(DD_RAM_ADDR+15);
  SEND_CHAR(48+right_points);
  //delay na szybkosc gry
  Delayx100us(127-game_speed);
  P1OUT &= ~BIT5;
  P1OUT &= ~BIT6;
 }
}
 
void end_game(short left_points, short right_points)
{
   clearDisplay();
   Delayx100us(1);
   SEND_CMD(DD_RAM_ADDR);
   SEND_CHAR(48+left_points);
   SEND_CMD(DD_RAM_ADDR+15);
   SEND_CHAR(48+right_points);
   SEND_CMD(DD_RAM_ADDR2);
   if(left_points>right_points)
     writeText("  LEWY WYGRAL!  ", 16);
   if(left_points<right_points)
     writeText("  PRAWY WYGRAL! ", 16);
   
   for(int i=4; i>=0; --i)
   {
     left_results[i] = left_results[i-1];
     right_results[i] = right_results[i-1];
   }
   left_results[0] = left_points;
   right_results[0] = right_points;
   
   Delayx100us(200);
   Delayx100us(200);
   Delayx100us(200);
   Delayx100us(200);
   Delayx100us(200);
   while(!(ANY_BUTTON_CLICKED));
   Delayx100us(200);
   return;
}


