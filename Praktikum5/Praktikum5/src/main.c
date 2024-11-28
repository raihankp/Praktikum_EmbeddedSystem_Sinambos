/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include <stdio.h>
#include <ioport.h>
#include <board.h>

static char strbuf[128];

// Interrupt Service Routine for PORTF INT0
ISR(PORTF_INT0_vect)
{
	// Your interrupt handler code here
	// For example, toggle an LED on PORTD Pin 0
	PORTR.OUTTGL = 0x01;  // Toggle the LED on PORTD Pin 0
	delay_ms(100);
	PORTR.OUTTGL = 0x01;
	delay_ms(100);
}

void init_external_interrupt(void)
{
	// Set PORTF Pin 1 as input (clear bit 1 in DIR register)
	PORTF.DIR &= ~(1 << 1);  // Clear bit 1 to set PORTF_PIN1 as input

	// Enable pull-up resistor on PORTF Pin 1 (PULLUP configuration in PIN1CTRL)
	PORTF.PIN1CTRL &= (3<<3); //(0x03 << 3);  // Set PULLUP bit in PIN1CTRL (bit 3)

	// Configure interrupt to trigger on rising edge (ISC setting in PIN1CTRL)
	PORTF.PIN1CTRL |= (0x03 << 0);  // Set ISC0 in PIN1CTRL (bit 0) for low level

	// Enable interrupt for PORTF Pin 1 (INT0MASK configuration)
	PORTF.INT0MASK = (1 << 1);  // Set bit 1 to enable INT0 for PORTF_PIN1

	// Set interrupt priority to medium level (INTCTRL configuration)
	PORTF.INTCTRL = (0x01 << 0);  // Set INT0LVL to low
	
	PMIC.CTRL |= 0x01; //PMIC_LOLVLEN_bm;

	// Enable global interrupts
	SREG |= (1<<7);
}

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */
	board_init();
	
	// Initialize external interrupt for PORTF Pin 1
	init_external_interrupt();
	
	// Set PORTR Pin 0 and 1 as output for LED
	PORTR.DIR |= (3 << 0);  // Set bit 0 in DIR register to set PORTD_PIN0 as output
	
	//init lcd
	gfx_mono_init();

	//set background lcd on
	gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);

	//set display - output lcd
	gfx_mono_draw_string("RaihanRadityaRafinal ",0, 0, &sysfont);
	gfx_mono_draw_string("Praktikum 05 : Interrupt",0, 8, &sysfont);
	
	int counter = 0;

	/* Insert application code here, after the board has been initialized. */
	while(1){
		counter++;
		bool btn = PORTF.IN & (1 << 1);
		if(!btn){
			PORTR.OUTTGL = 0x02;
		}
		
		snprintf(strbuf, sizeof(strbuf), "Counter : %3d", counter);
		gfx_mono_draw_string(strbuf,0, 24, &sysfont);
		
		delay_ms(500);
		
	}
}