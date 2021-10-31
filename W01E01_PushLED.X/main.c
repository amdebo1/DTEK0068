/*
 * File:   main.c
 * Author: Aleksander Debowski amdebo@utu.fi
 *
 * The program turns on the LED when button is pressed and
 * turns it off when the button is released
 */


#include <avr/io.h>

int main(void)
{
    // Set PF5 (LED) as out
    PORTF.DIRSET = PIN5_bm;
    // Set PF6 (Button) as in
    PORTF.DIRSET &= ~PIN6_bm;

    // The superloop
    while (1)
    {
        // If PF6 high turn the LED on
        if (PORTF.IN & PIN6_bm)
        {
            PORTF.OUT |= PIN5_bm;
        }
        else
        {
            PORTF.OUT &= ~PIN5_bm;
        }
        
    }
}