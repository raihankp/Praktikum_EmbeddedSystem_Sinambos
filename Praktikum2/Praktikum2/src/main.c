#include <asf.h>
#include <stdio.h>
#include <ioport.h>
#include <board.h>

static char strbuf[128];

void PWM_Init(void)
{
	/* Set output */
	PORTC.DIR |= PIN0_bm;

	/* Set Register */
	TCC0.CTRLA = PIN1_bm; //(PIN2_bm) | (PIN0_bm);
	TCC0.CTRLB = (PIN4_bm) | (PIN2_bm) | (PIN1_bm);
	
	/* Set Period */
	TCC0.PER = 1000;

	/* Set Compare Register value*/
	TCC0.CCA = 375;
}

void update_led_brightness(uint8_t counter) {
	uint16_t duty_cycle = (counter * 1000) / 7;  // Convert counter (0-7) into duty cycle value (0-1000)
	TCC0.CCA = duty_cycle;
}

int main(void) {
	/* Insert system clock initialization code here (sysclk_init()). */

	board_init();
	PWM_Init();
	
	// Initialize the LCD
	gfx_mono_init();
	
	// Set the background LCD on
	gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);

	// Display the user's name on the first line of the LCD
	gfx_mono_draw_string("Raihan\nRaditya\nRafinal", 0, 0, &sysfont);

	// Initialize counter
	uint8_t counter = 0;

	while(1) {
		// Check if SW1 is pressed (increment)
		if(ioport_get_pin_level(GPIO_PUSH_BUTTON_1) == 0) {
			counter = (counter + 1) % 8;  // Increment and wrap around at 8
			delay_ms(200);  // Debounce delay
		}
		// Check if SW2 is pressed (decrement)
		if(ioport_get_pin_level(GPIO_PUSH_BUTTON_2) == 0) {
			counter = (counter == 0) ? 7 : counter - 1;  // Decrement and wrap around at 0
			delay_ms(200);  // Debounce delay
		}

		// Change LED brightness based on counter value
		update_led_brightness(counter);

		// Display the counter value on the second line of the LCD
		snprintf(strbuf, sizeof(strbuf), "Counter: %d", counter);
		gfx_mono_draw_string(strbuf, 0, 24, &sysfont);

		// Small delay to avoid bouncing effects
		delay_ms(50);
	}
}
