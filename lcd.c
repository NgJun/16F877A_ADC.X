#include "lcd.h"

void LCD_Init()
{
    LCD_E_TRIS = 0;
    LCD_PORT_TRIS = 0;
    LCD_RS_TRIS=0;
    LCD_RW_TRIS = 0;
    
    LCD_Cmd(0x38);
    LCD_Cmd(0x0C);
    LCD_Cmd(0x06);
    LCD_Cmd(0x01);
    
}

void LCD_Cmd( char cmd)
{
    LCD_RW = 0;
    LCD_E = 0;
    LCD_RS = 0;
    PORTD = cmd;
    LCD_E = 1;
    __delay_us(5);
    LCD_E = 0;
    __delay_ms(10);
    
}

void LCD_Data(char data)
{
    LCD_RW = 0;
    LCD_E = 0;
    LCD_RS = 1;
    PORTD = data;
    LCD_E = 1;
    __delay_us(5);
    LCD_E = 0;
    __delay_ms(10);
}

void putString(const char *str)
{
    uint8_t i=0;
    while (str[i])
    {
        LCD_Data(str[i++]);
        
    }
    
}

void LCDGoto(uint8_t pos,uint8_t ln)
{
    // if incorrect line or column
    if ((ln > (NB_LINES-1)) || (pos > (NB_COL-1)))
    {
        // Just do nothing
        return;
    }

    // LCD_Goto command
    LCD_Cmd(   (ln == 1) ? (0xC0 | pos) : (0x80 | pos));

    // Wait for the LCD to finish
    __delay_ms(5);
}