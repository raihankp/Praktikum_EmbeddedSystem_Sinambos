#include <asf.h>
#include <stdio.h>
#include "FreeRTOS/include/FreeRTOS.h"
#include "FreeRTOS/include/queue.h"
#include "FreeRTOS/include/task.h"
#include "FreeRTOS/include/timers.h"
#include "FreeRTOS/include/semphr.h"

/* Define a task */
static portTASK_FUNCTION_PROTO(vBlinkLed0, p_);
static portTASK_FUNCTION_PROTO(vServo, q_);
static portTASK_FUNCTION_PROTO(vCounter, r_);
static portTASK_FUNCTION_PROTO(vPushButton1, s_);

/* Define semaphore */
SemaphoreHandle_t xSemaphore;
uint16_t counter = 0;

/* PWM configurations for servo */
void PWM_Init(void)
{
	/* Set output */
	PORTC.DIR |= PIN0_bm;	// PC0
	/* Set Register */
	TCC0.CTRLA = (PIN2_bm) | (PIN1_bm);		// Berdasarkan datasheet, 0110 adalah prescaler 256
	TCC0.CTRLB = (PIN4_bm) | (PIN2_bm) | (PIN1_bm);		// Berdasarkan datasheet, pin4 dan 110 adalah enable cca dan double-slope pwm
	/* Set Period */
	TCC0.PER = 156;		// didapat dari rumus PER = clock / (prescaler x frekuensi) dmn clocknya 2MHz (default system clock), prescaler 256, dan frekuensi 50hz (dari 20ms pulse cycle servo)
	/* Set Compare Register value*/
	TCC0.CCA = 1;
}

int main(void) {
	/* System clock initialization */
	sysclk_init();
	board_init();
	gfx_mono_init();

	gpio_set_pin_high(LCD_BACKLIGHT_ENABLE_PIN);
	gfx_mono_draw_string("RaihanRadityaRafinal", 0, 0, &sysfont);

	/* Create the task */
	xTaskCreate(vBlinkLed0, "", 1000, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(vServo, "", 1000, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(vPushButton1, "", 1000, NULL, tskIDLE_PRIORITY + 3, NULL);
	xTaskCreate(vCounter, "", 1000, NULL, tskIDLE_PRIORITY, NULL);

	/* Semaphore */
	xSemaphore = xSemaphoreCreateBinary();
	xSemaphoreGive(xSemaphore);

	/* Start the task */
	vTaskStartScheduler();
}

static portTASK_FUNCTION(vBlinkLed0, p_) {
	char strbuf[128];
	int flagLed0 = 1;

	while (1) {
		flagLed0 = !flagLed0;
		ioport_set_pin_level(LED0_GPIO, flagLed0);
		snprintf(strbuf, sizeof(strbuf), "LED 0 : %d", !flagLed0);
		gfx_mono_draw_string(strbuf, 0, 16, &sysfont);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

static portTASK_FUNCTION(vServo, q_) {
	char strbuf[128];
	/* Initialize PWM for servo control */
	PWM_Init();
	// variables for controlling servo
	uint16_t duty_cycle = 1;
	uint16_t direction = 1;
	
	while (1) {
		if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE) {
			TCC0.CCA = duty_cycle;
			
			// Update the duty cycle
			duty_cycle += direction;
			
			// Change direction if reach the ends
			// 1 for 0 Degress and 10 for 180 degrees
			if (duty_cycle >= 10) {
				direction = -1; // Change direction to decrease
				} else if (duty_cycle <= 1) {
				direction = 1; // Change direction to increase
			}

			// Display the duty cycle value to keep track
			snprintf(strbuf, sizeof(strbuf), "Servo Duty: %u", duty_cycle);
			gfx_mono_draw_string(strbuf, 0, 24, &sysfont);

			xSemaphoreGive(xSemaphore);
		}
		vTaskDelay(20 / portTICK_PERIOD_MS); // Adjust delay for smooth movement
	}
}

static portTASK_FUNCTION(vPushButton1, s_) {
	char strbuf[128];

	while (1) {
		if (ioport_get_pin_level(GPIO_PUSH_BUTTON_1) == 0) {
			if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE) {
				counter++;
				snprintf(strbuf, sizeof(strbuf), "Counter : %d", counter);
				gfx_mono_draw_string(strbuf, 0, 8, &sysfont);
				xSemaphoreGive(xSemaphore);
			}
		}

		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

static portTASK_FUNCTION(vCounter, r_) {
	char strbuf[128];

	while (1) {
		if (xSemaphoreTake(xSemaphore, (TickType_t)10) == pdTRUE) {
			counter++;
			snprintf(strbuf, sizeof(strbuf), "Counter : %d", counter);
			gfx_mono_draw_string(strbuf, 0, 8, &sysfont);
			xSemaphoreGive(xSemaphore);
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}
