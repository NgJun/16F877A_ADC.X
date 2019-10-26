#include "adc.h"



void Delay_adc(int cnt)
{
    while(cnt--);
}
void Init_ADC()
{ 
    //  Fosc/64 =  20/64 Mhz
    ADCON0bits.ADCS0 = 0;
    ADCON0bits.ADCS1 = 1;
    ADCON1bits.ADCS2 = 1;
    
    ADCON0bits.CHS0=0;
    ADCON0bits.CHS1=0;
    ADCON0bits.CHS2=0;
    
    ADCON1bits.ADFM = 1;
    ADCON1bits.PCFG0 = 0;
    ADCON1bits.PCFG1 = 0;
    ADCON1bits.PCFG2 = 0;
    ADCON1bits.PCFG3 = 0;
    
    ADCON0bits.GO_DONE = 1;
    ADCON0bits.ADON = 1;
}
