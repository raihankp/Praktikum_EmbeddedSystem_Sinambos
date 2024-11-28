#include <asf.h>
#include <stdio.h>

static char strbuf[128];

int main (void)
{
	// Nama Anggota Kelompok:
	// Muhammad Raditya Hanif (2106750585)
	// Rafinal Haryokusumo Taloputra(2106634540)
	// Raihan Kus Putranto (2106703821)
	
	/* Insert system clock initialization code here (sysclk_init()). */
	
	// Initialize the board
	board_init();
	
	// Initialize the LCD
	gfx_mono_init();
	
	// Set the background LCD on
	gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);
	
	// Display the user's name on the first line of the LCD
	gfx_mono_draw_string("Raihan\nRaditya\nRafinal", 0, 0, &sysfont);
	
	// Counter value (in base 8)
	uint8_t counter = 0;
	
	// Forever loop
	while(1){
		// Check if SW1 is pressed (increment)
		if(ioport_get_pin_level(GPIO_PUSH_BUTTON_1) == 0){
			counter = (counter + 1) % 8;  // Increment and wrap around at 8
			delay_ms(200);  // Debounce delay
		}
		// Check if SW2 is pressed (decrement)
		if(ioport_get_pin_level(GPIO_PUSH_BUTTON_2) == 0){
			counter = (counter == 0) ? 7 : counter - 1;  // Decrement and wrap around at 0
			delay_ms(200);  // Debounce delay
		}
		
		// Set LEDs based on counter value in binary
		ioport_set_pin_level(LED2_GPIO, !(counter & 0x01));
		ioport_set_pin_level(LED1_GPIO, !((counter >> 1) & 0x01));
		ioport_set_pin_level(LED0_GPIO, !((counter >> 2) & 0x01));
		
		// Display the counter value on the second line of the LCD
		snprintf(strbuf, sizeof(strbuf), "Counter: %d", counter);
		gfx_mono_draw_string(strbuf, 0, 24, &sysfont);
		
		// Small delay to avoid bouncing effects
		delay_ms(50);
	}
}