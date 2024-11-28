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

	/* Set Compare Register value for initial position (0 degrees) */
	TCC0.CCA = 50;
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

	// variables for controlling servo
	uint16_t duty_cycle = 50; // Start at 0 degrees
	int8_t direction = 1; // 1 for increasing, -1 for decreasing

	while(1) {
		TCC0.CCA = duty_cycle * 10;
		
		// Update the duty cycle
		duty_cycle += direction;
		
		// Change direction if reach the ends
		// 50 for 0 Degress and 84 for 180 degrees
		if (duty_cycle >= 84) {
			direction = -1; // Change direction to decrease
			} else if (duty_cycle <= 50) {
			direction = 1; // Change direction to increase
		}

		// Display the duty cycle value to keep track
		snprintf(strbuf, sizeof(strbuf), "Duty: %u", duty_cycle);
		gfx_mono_draw_string(strbuf, 0, 24, &sysfont);

		delay_ms(20);
	}
}
