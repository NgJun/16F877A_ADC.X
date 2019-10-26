/*
 * Read and Send adc value to hc06
 * File:   main.c
 * Author: JBsnipper
 * Ref: Heart "Beat Monitoring using PIC Microcontroller and Pulse Sensor" on circuitdigest.com
 * Created on October 12, 2019, 4:23 PM
 */

#include "config.h"
#include <xc.h>
#include "adc.h"
#include "lcd.h"
#include "bluetooth.h"

volatile int rate[10]; 
volatile unsigned long sampleCounter = 0; 
volatile unsigned long lastBeatTime = 0; 
volatile int P = 512;
volatile int T = 512; 
volatile int thresh = 530;
volatile int amp = 0; 
volatile bool firstBeat = true; 
volatile bool secondBeat = false; 

volatile int BPM; // int that holds raw Analog in 0. updated every 2mS
volatile int Signal;                   // holds the incoming raw data
volatile int IBI = 600;             // int that holds the time interval between beats! Must be seeded!
volatile bool Pulse = false;    // "True" when User's live heartbeat is detected. "False" when not a "live beat".
volatile bool QS = false;       // becomes true when finds a beat.

int main_state = -1;
int adc_value = 0;
int tune = 0;


void Pin_Init()
{
    TRISA = 0xFF;
    TRISB = 0;
    PORTB = 0xFF;
    __delay_ms(200);
    PORTB = 0;
    TRISD = 0;       
}

void calculate_heart_beat(int adc_value) 
{

    Signal = adc_value;

    sampleCounter += 2;                                 // keep track of the time in mS with this variable
    int N = sampleCounter - lastBeatTime;      // monitor the time since the last beat to avoid noise

    //  find the peak and trough of the pulse wave
    if (Signal < thresh && N > (IBI / 5)*3) 
    { // avoid dichrotic noise by waiting 3/5 of last IBI
        if (Signal < T) 
        { 
            // T is the trough
            T = Signal; // keep track of lowest point in pulse wave
        }
    }

    if (Signal > thresh && Signal > P) 
    { // thresh condition helps avoid noise
        P = Signal; // P is the peak
    } // keep track of highest point in pulse wave

    //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
    // signal surges up in value every time there is a pulse
    if (N > 250) 
    { 
        // avoid high frequency noise
        if ((Signal > thresh) && (Pulse == false)  &&  (N > (IBI / 5)*3)) 
        {
            Pulse = true; // set the Pulse flag when we think there is a pulse
            IBI = sampleCounter - lastBeatTime; // measure time between beats in mS
            lastBeatTime = sampleCounter; // keep track of time for next pulse

            if (secondBeat) { // if this is the second beat, if secondBeat == TRUE
                secondBeat = false; // clear secondBeat flag
                int i;
                for (i = 0; i <= 9; i++) { // seed the running total to get a realisitic BPM at startup
                    rate[i] = IBI;
                }
            }

            if (firstBeat) { // if it's the first time we found a beat, if firstBeat == TRUE
                firstBeat = false; // clear firstBeat flag
                secondBeat = true; // set the second beat flag
                //pulse_tmr_handle = bsp_harmony_start_tmr_cb_periodic(PULSE_CHECK_TIME_INTERVAL, 0, pulse_read_cb); // enable interrupts again
                return; // IBI value is unreliable so discard it
            }


            // keep a running total of the last 10 IBI values
            uint8_t runningTotal = 0; // clear the runningTotal variable
            int i;
            for (i = 0; i <= 8; i++) 
            { // shift data in the rate array
                rate[i] = rate[i + 1]; // and drop the oldest IBI value
                runningTotal += rate[i]; // add up the 9 oldest IBI values
            }

            rate[9] = IBI; // add the latest IBI to the rate array
            runningTotal += rate[9]; // add the latest IBI to runningTotal
            runningTotal /= 10; // average the last 10 IBI values
            BPM = 60000 / runningTotal; // how many beats can fit into a minute? that's BPM!
            QS = true; // set Quantified Self flag
            /*
             * 75 x runningTotal = 60000 
             
             */
            // QS FLAG IS NOT CLEARED INSIDE THIS ISR
        }
    }
    
if (Signal < thresh && Pulse == true) 
{ // when the values are going down, the beat is over
        Pulse = false; // reset the Pulse flag so we can do it again
        amp = P - T; // get amplitude of the pulse wave
        thresh = amp / 2 + T; // set thresh at 50% of the amplitude
        P = thresh; // reset these for next time
        T = thresh;
 }

    if (N > 2500) 
    { // if 2.5 seconds go by without a beat
        thresh = 530; // set thresh default
        P = 512; // set P default
        T = 512; // set T default
        lastBeatTime = sampleCounter; // bring the lastBeatTime up to date
        firstBeat = true; // set these to avoid noise
        secondBeat = false; // when we get the heartbeat back
    }

}

void main(void) {
    Pin_Init();
    Init_ADC();
    LCD_Init();
    Initialize_Bluetooth();
        
    int blt_mess;
    BT_load_string("Bluetooth Initialized and Ready");
    broadcast_BT();putString("BLT Ready");  __delay_ms(100); LCDGoto(0,0);
  
    char mess[20];
    float data = 0;
    int trans = 0;
    int i=0;
    while(1)
    {
        blt_mess = BT_get_char();
        if( blt_mess == '1' ) PORTB = 0xFF;
        if( blt_mess == '0' ) PORTB = 0;
       if(ADCON0bits.GO_DONE == 0 ) 
        {
            ADCON0bits.GO_DONE = 1;
            data =  ((ADRESH << 8) + ADRESL);  //10-bit result
            data = data/1023*100; // chuan hoa ve 5V
            
            sprintf(mess, "%.0f\n",data);
            LCDGoto(0,0);
            BT_load_string(mess);
            
            broadcast_BT();
            putString(mess);
         
        }
    };
    return;
}
