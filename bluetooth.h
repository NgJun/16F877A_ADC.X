#include "config.h"

#define blt_delay 500
void Initialize_Bluetooth();//Initialize Bluetooth using USART
void BT_load_char(char byte);
void BT_load_string(char* string);//Function to load the Bluetooth Rx. buffer with one char.
void broadcast_BT();
char BT_get_char(void); //Function to get a char from Rx.buffer of BT


