/*
 * File:   main.c
 * Author: Aleksander Debowski amdebo@utu.fi
 *
 * The program uses 7-segment LED display to countdown from 9 to 0
 * If it reaches 0 it will start blinking that value. If the wire connected from
 * PIN4 in PORTA to the ground is cut the countdown will stop. This version of the program
 * doesn't use delays but rather than that it uses RTC.
 */

#include <avr/io.h>
#include <avr/cpufunc.h> // for ccp_write_io()
#include <avr/interrupt.h>
#include <avr/sleep.h>

void rtc_init();

volatile uint8_t g_running = 1;  //global variable that specifies if the countdown is still running
volatile uint8_t g_clockticks = 0;
//ISR for PortA that switches g_running to 0
ISR(PORTA_PORT_vect)
{
    PORTA.INTFLAGS = 0xFF;
    g_running = 0;
}



void rtc_init(void)
{
    uint8_t temp;
    
    /* Initialize 32.768kHz Oscillator: */
    /* Disable oscillator: */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_ENABLE_bm;
    /* Writing to protected register */
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    while(CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
    {
        ; /* Wait until XOSC32KS becomes 0 */
    }
    
    /* SEL = 0 (Use External Crystal): */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp &= ~CLKCTRL_SEL_bm;
    /* Writing to protected register */
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    /* Enable oscillator: */
    temp = CLKCTRL.XOSC32KCTRLA;
    temp |= CLKCTRL_ENABLE_bm;
    /* Writing to protected register */
    ccp_write_io((void*)&CLKCTRL.XOSC32KCTRLA, temp);
    
    /* Initialize RTC: */
    while (RTC.STATUS > 0)
    {
        ; /* Wait for all register to be synchronized */
    }

    /* 32.768kHz External Crystal Oscillator (XOSC32K) */
    RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc;

    /* Run in debug: enabled */
    RTC.DBGCTRL = RTC_DBGRUN_bm;
    
    RTC.PITINTCTRL = RTC_PI_bm; /* Periodic Interrupt: enabled */
    
    RTC.PITCTRLA = RTC_PERIOD_CYC4096_gc /* RTC Clock Cycles 4096 which corresponds to 1/8 sec */
                 | RTC_PITEN_bm; /* Enable: enabled */
}

ISR(RTC_PIT_vect)
{
    /* Clear flag by writing '1': */
    RTC.PITINTFLAGS = RTC_PI_bm;
    g_clockticks = 1;
}


int main(void)
{
    // Set PORTC as out
    PORTC.DIRSET = 0xFF;
    // Set PF5 (transistor control) as out
    PORTF.DIRSET = PIN5_bm;
    //SET PIN4A as in
    PORTA.DIRSET &= ~PIN4_bm;
    //Enable pull up resistor and trigger interrupt on rising edge
    PORTA.PIN4CTRL = PORT_PULLUPEN_bm | PORT_ISC_RISING_gc;
    //Array of numbers from 0 to 9 to be used by the display + the last number for the display to be off
    uint8_t numbers[] =
    {
        0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110,
        0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111,
        0b00000000
    };
    
    //index for the countdown
    uint8_t index = 9;
    uint8_t ticksCounter = 0;
    rtc_init();
    
    set_sleep_mode(SLPCTRL_SMODE_IDLE_gc);
    
    sei();
    
    PORTF.OUT |= PIN5_bm;
    
    while (1)
    {
        //Set the display to the number that corresponds to the index value
        PORTC.OUT = numbers[index];
        if (g_running == 1)
        {
            
            if (g_clockticks == 1)
            {
                g_clockticks = 0;
                ticksCounter += 1;
                
                //If the counter is equal to 8 it means that 1sec passed
                if (ticksCounter == 8)
                {
                    ticksCounter = 0;
                    if (index > 0)
                    {
                        index--;
                    }
                }
                //If we got to 0 start blinking the display
                if (index == 0)
                    {
                        PORTF.OUTTGL |= PIN5_bm;
                    }
                
            }

        }
        
        sleep_mode();
    }
}