#include "config.h"

#define LCD_CLEAR           0x01
#define LCD_RS_TRIS TRISB1
#define LCD_RW_TRIS TRISB2
#define LCD_PORT_TRIS TRISD
#define LCD_E_TRIS TRISB3

#define LCD_RS RB1
#define LCD_RW RB2
#define LCD_E RB3
//#define LCD_PORT_TRIS PORTD

#define NB_LINES    2                                   // Number of display lines
#define NB_COL      16                                  // Number of characters per line


#define DisplayClr()        LCD_Cmd(LCD_CLEAR)  

void LCD_Init();
void LCD_Cmd(char cmd);
void LCD_Data(char data);
void putString(const char *str);
void LCDGoto(uint8_t pos,uint8_t ln);