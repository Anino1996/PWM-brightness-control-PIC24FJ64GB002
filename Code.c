/*
 * File:   ICOC.c
 * Author: anino1996
 *
 * Created on April 25, 2020, 5:17 AM
 */

#include <p24FJ64GB002.h>
#include "xc.h"


#define TMRPERIOD 0x270     // Corresponding to a 20ms period for a prescaler value of 64
volatile long PWidth;       // Pulse width intermediate variable to be used in ISR

void analogConfig()
{
    AD1CON1 = 0x04E0;       // Set Analog-Digital register to 12-bit, Manual sampling and autoconvert modes
    CNPU1bits.CN2PUE=0;     // Ensure Change notification pull-up is disabled for analog pin.
    TRISAbits.TRISA0=1;     // Set pin AN0 as an input
    AD1PCFGbits.PCFG0=0;    // Set pin AN0 as analog bit.
    AD1CON2bits.VCFG=0x0;   // Configure reference voltages as VDD and Vss
    AD1CON3bits.ADRC=1;     // Use A/D internal clock
    AD1CON3bits.SAMC=0x1F;  // Set sampling time to 31 internal clock cycles
    AD1CHSbits.CH0SA=0x00;  // Set AN0 bit as positive analog input pin.
    AD1CHSbits.CH0NA=0;     // Set reference pin as Vss (Or whichever negative reference was chosen per AD1CON2 settings)
    AD1CON1bits.ADON=1;     // Turn on analog digital converter
    
}

void setupTMR2()
{   
    PR2 = TMRPERIOD;        // Set Timer 2 period register such that T2 interrupt is triggered after 20ms
    IPC1bits.T2IP = 0x7;    // Set Timer 2 interrupt priority to 7
    T2CON = 0x8020;         // Enable Timer 2 with a prescaler ratio of 1:64
    IFS0bits.T2IF = 0;      // Clear Timer 2 interrupt flag
    IEC0bits.T2IE = 1;      // Enable Timer 2 interrupts
    TMR2 = 0;               // Reset Timer 2 value
}

void configure_OC(void){
    OC1CON1bits.OCM = 0x7;  // Set OC1 in PWM mode
    OC1CON1bits.OCTSEL=0x0; //  Use timer 2 as output compare timer
    OC1R = 0;               //  Initialize OC1 primary register at TMR2 = 0
    OC1RS = 0x138;          //  Initialize OC1 secondary register at half the timer period. (Half pulse width)
    RPOR7bits.RP14R=18;     //  Map OC1 output to RP14
}
  



void __attribute__((__interrupt__)) _T2Interrupt(void){
   
    while(!AD1CON1bits.DONE){}; // Check if Analog-digital conversion is complete
    PWidth = ADC1BUF0;          // Copy value saved in A/D buffer to intermediate variable
    PWidth = (PWidth * PR2)>>12; //Scale analog value as a fraction of PR2
    OC1RS=PWidth;           // Update Secondary OC1 register with new pulse width value 
    AD1CON1bits.SAMP=1;     // Start analog input sampling
    TMR2 = 0;               // Reset timer 2
    IFS0bits.T2IF=0;        // Clear Timer 2 interrupt flag
    }
  


int main(void) {
    configure_OC();         // Execute output compare configuration subroutine 
    analogConfig();         // Execute analog converter setup subroutine
    setupTMR2();            // Execute timer setup subroutine
    AD1CON1bits.SAMP=1;     // Start sampling at analog pin 0
    while(1);               // Do nothing
    
    return 0;
}
