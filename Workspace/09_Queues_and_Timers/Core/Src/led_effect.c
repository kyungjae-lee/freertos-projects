/* led_effect.c */

#include "main.h"

// Stops the active LED timers
void stop_led_effect(void)
{
	for (int i = 0; i < 4; i++)
	{
		xTimerStop(led_timer_handles[i], portMAX_DELAY);
	}
}

// Starts the timer
void led_effect(int n)
{
	stop_led_effect();

	// Start the corresponding timer
	xTimerStart(led_timer_handles[n - 1], portMAX_DELAY);
		// When the started timer expires, the led_timer_callback() function will
		// be invoked, a proper led effect function will get called and the
		// LEDs will be controlled accordingly.
}

void turn_off_all_leds(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
}

void turn_on_all_leds(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
}

void turn_on_odd_leds(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_RESET);
}

void turn_on_even_leds(void)
{
	HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LD5_GPIO_Port, LD5_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LD6_GPIO_Port, LD6_Pin, GPIO_PIN_SET);
}

void led_control(int value)
{
	for (int i = 0; i < 4; i++)
	{
		HAL_GPIO_WritePin(LD3_GPIO_Port, (LED1 << i), ((value >> i) & 0x1));
	}
}

// Effect 1: All Leds toggling every 500ms
void led_effect1(void)
{
	static int flag = 1;
	(flag ^= 1) ? turn_off_all_leds() : turn_on_all_leds();
}

// Effect 2: Even, odd toggling every 500ms
void led_effect2(void)
{
	static int flag = 1;
	(flag ^= 1) ? turn_on_even_leds() : turn_on_odd_leds();
}

// Effect 3: 1 -> 2 -> 3 -> 4 -> 1 -> ... every 500ms
void led_effect3(void)
{
	static int i = 0;
	led_control(0x1 << (i++ % 4));
}

// Effect 4: 4 -> 3 -> 2 -> 1 -> 4 -> ... every 500ms
void led_effect4(void)
{
	static int i = 0;
	led_control(0x8 >> (i++ % 4));
}
