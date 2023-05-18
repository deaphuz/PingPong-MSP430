#include "lcd.h"

void writeText(unsigned char *text, short length)
{
  for(int i=0; i<length; i++)
    SEND_CHAR(text[i]);
}

void scrollText(unsigned char *text, short offset)
{
  for(int i=0; i<16; i++)
    SEND_CHAR(text[i+offset]);
}
