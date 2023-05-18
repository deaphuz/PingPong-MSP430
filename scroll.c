#include "lcd.h"
#include "portyLcd.h"
#include "text.h"
#include<msp430x14x.h>

#define ANY_BUTTON_CLICKED !((P4IN & BIT4)==BIT4) || !((P4IN & BIT5)==BIT5) || !((P4IN & BIT6)==BIT6)  ||  !((P4IN & BIT7)==BIT7)



void authors()
{
  unsigned char* Authors1=" Marcin Aleksiejuk, Jakub Cimoch, Jacek Grzybowski, Krystian Borys   ";
  Delayx100us(200);
  unsigned char offset = 0;
  while(1)
  {
    clearDisplay();
    Delayx100us(1);
    scrollText(Authors1, ++offset);
    for(long int i=50; i>0; --i)
    {
      Delayx100us(3);
      if(ANY_BUTTON_CLICKED || offset>50)
      {
        Delayx100us(200);
        Delayx100us(200);
        Delayx100us(200);
        Delayx100us(200);
        return;
      }
    }
  }
}

void description()
{
  Delayx100us(200);
    clearDisplay();
    Delayx100us(1);
    writeText("L:B1:Up, B2:down", 16);
    Delayx100us(1);
    SEND_CMD(DD_RAM_ADDR2);
    writeText("P:B3:Up, B4:down", 16);
  while(1)
  {

      Delayx100us(2);
      if(ANY_BUTTON_CLICKED)
      {
        Delayx100us(200);
        return;
      }
   }
}