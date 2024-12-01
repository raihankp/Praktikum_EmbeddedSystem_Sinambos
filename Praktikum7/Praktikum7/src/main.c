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
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include <asf.h>
#include <stdio.h>
#include <string.h>
#include <ioport.h>


#define USART_SERIAL_EXAMPLE             &USARTC0
#define USART_SERIAL_EXAMPLE_BAUDRATE    9600
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false

static char strbuf[201];

void setUpSerial()
{
    // Baud rate selection
    // BSEL = (2000000 / (2^0 * 16*9600) -1 = 12.0208... ~ 12 -> BSCALE = 0
    // FBAUD = ( (2000000)/(2^0*16(12+1)) = 9615.384 -> mendekati lah ya
   
    USARTC0_BAUDCTRLB = 0; //memastikan BSCALE = 0
    USARTC0_BAUDCTRLA = 0x0C; // 12
	
	//USARTC0_BAUDCTRLB = 0; //Just to be sure that BSCALE is 0
	//USARTC0_BAUDCTRLA = 0xCF; // 207
   
    //Disable interrupts, just for safety
    USARTC0_CTRLA = 0;
    //8 data bits, no parity and 1 stop bit
    USARTC0_CTRLC = USART_CHSIZE_8BIT_gc;
   
    //Enable receive and transmit
    USARTC0_CTRLB = USART_TXEN_bm | USART_RXEN_bm; 
}

void sendString(char *text)
{
    while(*text)
    {
		usart_putchar(USART_SERIAL_EXAMPLE, *text++);
		delay_ms(20);
    }
}

int main (void)
{
    // Insert system clock initialization code here (sysclk_init()).
	//sysclk_init();
    board_init();
    gfx_mono_init();
   
    gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);
	
	gfx_mono_draw_string("RaihanRadityaRafinal",0, 0, &sysfont);
	gfx_mono_draw_string("Praktikum 7",0, 8, &sysfont);
   
    PORTC_OUTSET = PIN3_bm; // PC3 as TX
    PORTC_DIRSET = PIN3_bm; //TX pin as output
   
    PORTC_OUTCLR = PIN2_bm; //PC2 as RX
    PORTC_DIRCLR = PIN2_bm; //RX pin as input
   
    setUpSerial();
    
    // Create USART Options
	static usart_rs232_options_t USART_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_EXAMPLE_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	
    // Init the USART
	usart_init_rs232(USART_SERIAL_EXAMPLE, &USART_SERIAL_OPTIONS);
	   
    while(1){	   
        if(gpio_pin_is_low(GPIO_PUSH_BUTTON_1) && gpio_pin_is_high(GPIO_PUSH_BUTTON_2)){
            // Turn LED 1 On and LED 2 Off if SW1 is pressed and send the string
            gpio_set_pin_low(LED1_GPIO);
            gpio_set_pin_high(LED2_GPIO);
            sendString("LED 1 On LED 2 Off\n");
        }else if(gpio_pin_is_low(GPIO_PUSH_BUTTON_2) && gpio_pin_is_high(GPIO_PUSH_BUTTON_1)){
            // Turn LED 1 Off and LED 2 On if SW2 is pressed and send the string
            gpio_set_pin_low(LED2_GPIO);
            gpio_set_pin_high(LED1_GPIO);
            sendString("LED 1 Off LED 2 On\n");
        }else if(gpio_pin_is_high(GPIO_PUSH_BUTTON_1) && gpio_pin_is_high(GPIO_PUSH_BUTTON_2)){
            // Turn LED 1 Off and LED 2 Off if no button is pressed and send the string
            gpio_set_pin_high(LED2_GPIO);
            gpio_set_pin_high(LED1_GPIO);
            sendString("LED 1 Off LED 2 Off\n");
        }
        delay_ms(50);
    }
}