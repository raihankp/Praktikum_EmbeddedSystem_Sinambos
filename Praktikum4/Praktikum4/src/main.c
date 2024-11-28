#include <asf.h>
#include <stdio.h>
#include <ioport.h>
#include <board.h>

static char strbuf[128];

uint16_t map(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max) {
	return ((uint32_t)(x - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min;
}


// Function to initialize ADC for potentiometer reading
void adc_init() {
	ADCB.CTRLA = 0x01;
	ADCB.CTRLB = 0x00;
	ADCB.REFCTRL = 0x02;
	ADCB.PRESCALER = 0b0000111;
}

// Read value from ADC
uint16_t read_adc_ch0() {
	// Start a conversion on ADC channel 0
	ADCB.CH0.MUXCTRL = 0x00; // Select ADC pin 0
	ADCB.CH0.CTRL |= 0x81;

	// Wait for conversion to complete
	while (!(ADCB.INTFLAGS & 0x01)); // Wait for interrupt flag

	// Read the conversion result
	uint16_t result = ADCB.CH0RES;
	
	return result;
}

void PWM_Init(void)
{
	/* Set output */
	PORTC.DIR |= PIN0_bm;

	/* Set Register */
	TCC0.CTRLA = PIN1_bm;;
	TCC0.CTRLB = (PIN4_bm) | (PIN2_bm) | (PIN1_bm);
	
	/* Set Period */
	TCC0.PER = 1000;

	/* Set Compare Register value for initial position (0 degrees) */
	TCC0.CCA = 50;
}

int main(void) {
	/* Insert system clock initialization code here */
	board_init();
	gfx_mono_init();
	
	gfx_mono_draw_string("RaihanRadityaRafinal ",0, 0, &sysfont);
	
	/* Initialize PWM for servo control */
	PWM_Init();
	
	/* Initialize ADC for potentiometer */
	adc_init();
	
	/* Enable LCD backlight */
	ioport_set_pin_level(LCD_BACKLIGHT_ENABLE_PIN, 1);
	
	uint16_t duty_cycle = 1; // Start at 0 degrees (1ms pulse)
	
	while (1) {
		// Read ADC value from potentiometer
		uint16_t adc_result = read_adc_ch0();
		
		// Map ADC value (0-4095) to PWM duty cycle (50 to 84)
		duty_cycle = map(adc_result, 0, 4095, 50, 84);
		
		// Update PWM duty cycle to control servo position
		TCC0.CCA = duty_cycle * 10;
		
		// Display the ADC and Duty Cycle value on LCD
		snprintf(strbuf, sizeof(strbuf), "ADC: %d", adc_result);
		gfx_mono_draw_string(strbuf, 0, 8, &sysfont);
		
		snprintf(strbuf, sizeof(strbuf), "Duty: %d", duty_cycle);
		gfx_mono_draw_string(strbuf, 0, 16, &sysfont);
		
		// Delay for servo movement stabilization
		delay_ms(20);
	}
}