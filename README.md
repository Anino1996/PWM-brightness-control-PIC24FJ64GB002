# PWM-control
Using the PIC24 microcontroller to control the width of a 20ms PWM in order to control LED brightness. The output compare module OC1 and the the analog-to-digital converter AD1 is used in this application to control an OC1 output configured at RB14. A timer 2 interrupt is used to change the value of the secondary output compare module 1 register based on an analog input received from a potentiometer connected to analog pin AN0.
The microcontroller checks first if analog-digital data conversion by the ADC is complete and the accurate data is available in the buffer. This is to avoid updating OC1RS with innacurate data in the buffer during conversion. 

Complete C code can be found in "Code.c" and a flowchart of the implement has been provided in "Flowchart.png". 
