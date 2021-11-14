#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/cpufunc.h>

void RTC_init(void);
void LED0_init(void);
inline void LED0_toggle(void);

uint8_t g_running = 1;
volatile uint8_t g_clockticks = 0;

void RTC_init(void)
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
    
    RTC.PITCTRLA = RTC_PERIOD_CYC32768_gc /* RTC Clock Cycles 32768 */
                 | RTC_PITEN_bm; /* Enable: enabled */
}

void LED0_init(void)
{
    /* Make High (OFF) */
    PORTF.OUT |= PIN5_bm;
    /* Make output */
    PORTF.DIR |= PIN5_bm;
}

inline void LED0_toggle(void)
{
    PORTF.OUTTGL |= PIN5_bm;
}

ISR(RTC_PIT_vect)
{
    g_clockticks = 1;
    /* Clear flag by writing '1': */
    RTC.PITINTFLAGS = RTC_PI_bm;
    
    
}

int main(void)
{
    LED0_init();
    RTC_init();
    
    /* Enable Global Interrupts */
    sei();
    
    while (1) 
    {
       
            if (g_clockticks == 1)
            {
                g_clockticks = 0;
                LED0_toggle();
                
            }
    }
}