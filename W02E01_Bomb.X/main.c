/*
 * File:   main.c
 * Author: Aleksander Debowski amdebo@utu.fi
 *
 * The program uses 7-segment LED display to countdown from 9 to 0
 * If it reaches 0 it will start blinking that value. If the wire connected from
 * PIN4 in PORTA to the ground is cut the countdown will stop
 */


#define F_CPU   3333333

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

uint8_t g_running = 1;  //global variable that specifies if the countdown is still running

//ISR for PortA that switches g_running to 0
ISR(PORTA_PORT_vect)
{
    PORTA.INTFLAGS = 0xFF;
    g_running = 0;
}

int main(void)
{
    // Set PORTC as out
    PORTC.DIRSET = 0xFF;
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
    
    sei();
    while (1)
    {
        //Set the display to the number that corresponds to the index value
        PORTC.OUT = numbers[index];
        if (g_running == 1)
        {
            _delay_ms(1000);
            //We check again the variable g_running in case if the interrupt was triggered during the delay
           if (g_running == 1 && index != 0)
           {
               index--;
           }
           //if the index is already 0 turn the display off and wait for a second
           else
           {
               PORTC.OUT = numbers[10];
               _delay_ms(1000);
           }           
        }
     
    }
}