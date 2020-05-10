/*
 * File:   main.c
 * Author: Stijn
 *
 * Created on 9 may 2020, 12:07
 */


#include <xc.h>                                     //PIC hardware mapping
#define _XTAL_FREQ 500000                            //Used by the XC8 delay_ms(x) macro

//config bits that are part-specific for the PIC16F1829
__CONFIG(FOSC_INTOSC & WDTE_OFF & PWRTE_OFF & MCLRE_OFF & CP_OFF & CPD_OFF & BOREN_ON & CLKOUTEN_OFF & IESO_OFF & FCMEN_OFF);
__CONFIG(WRT_OFF & PLLEN_OFF & STVREN_OFF & LVP_OFF);

    /* -------------------LATC-----------------
     * Bit#:  -7---6---5---4---3---2---1---0---
     * LED:   ---------------|DS4|DS3|DS2|DS1|-
     *-----------------------------------------
     */

//Valves
#define VALVES PORTA 
#define LEDS LATC //Debug module only
#define ISBLOWING (ADRES >> 7) & 1

#define TRUE 1

void init(void);
void setPwm(int);
void setKey(void);

const int KEYS[] = {64, 128, 192, 256, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1024}; //Lookup table for the trumpet keys

void main(void) {
    init();
    
    while (TRUE) setKey();
}

void init(void)
{
    //timing
    OSCCONbits.SPLLEN = 0;                          //Software PLL Enable
    OSCCONbits.IRCF = 0b0111;                       //Internal Oscillator Frequency Select. 500kHz
    OSCCONbits.SCS = 0b00;                          //System Clock Select. Clock determined by FOSC.
    
    //Microphone setup (ADC)
    TRISAbits.TRISA4 = 1;                           //Potentiometer is connected to RA4...set as input
    ANSELAbits.ANSA4 = 1;                           //analog
    ADCON0bits.CHS = 0b0000;                        //Select RA0 as source of ADC
    ADCON0bits.ADON = 1;                            //Enable ADC
    ADCON1 = 0b00010000;                            //left justified - FOSC/8 speed - Vref is Vdd

    //Buzzer setup (PWM)
    CCP2CON = 0b00001100;                           //PWM mode single output
    PR2 = 255;                                      //Frequency at 486Hz. Anything over ~60Hz will get rid of any flicker
                                                    //PWM Period = [PR2 + 1]*4*Tosc*T2CKPS = [255 + 1] * 4 * (1 / 500KHz) * 1
    CCPTMRSbits.C2TSEL = 0b00;                      //select timer2 as PWM source
    T2CONbits.T2CKPS = 0b00;                        //1:1 prescaler
    //T2CONbits.TMR2ON = 1;                           //start the PWM

    //Valves setup
    TRISC = 0;                                      //all LED pins are outputs
    LATC = 0;                                       //start with all LEDs OFF
    
    TRISA = 0b1111;                                 //switch as input
    ANSELA = 0b0000;                                //digital switch
}

void setPwm(int pwm)
{
    CCPR2L = pwm>>8;                            //put the top 8 MSbs into CCPR2L
    CCP2CONbits.DC2B = (pwm<<8>>6);             //put the 2 LSbs into DC2B register to complete the 10bit resolution
}

void setKey(void)
{
    while(ISBLOWING)
    {
        int valves = VALVES & 0b1111;                //get the 4 valves
        int keys = KEYS[keys];                       //get the key correspondending with the 
    
        setPwm(keys);
    }
    setPwm(0);
}