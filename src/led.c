// Eingebettete Systeme  / Embedded Systems
// Embedded Distance Assistent (EDA)
// SEMESTER: SS24
// AUTHORS: David Heiss, Manfred Pester, Jens-Peter Akelbein
// FILE: led.c
// CONTENTS: Source for Due Onboard LED control

#include <clock.h>
#include <constants.h>
#include <led.h>
#include <sam3x8e.h>
#include <state.h>

// BUILTIN-LED on Arduino Due Board on PIO Port B Bit 27
#define DUE_LED_BUILTIN PIO_PB27

// return state of LED
bool led_get(void) { 
	return (PIOB->PIO_ODSR & PIO_ODSR_P27);//>> PIO_ODSR_P27 != 0;
}

// turn on/off LED
void led_set(bool led_on) {
	if(led_on){
		PIOB->PIO_SODR |= PIO_SODR_P27; 
	} else {
		PIOB->PIO_CODR |= PIO_CODR_P27;
	}
}

// toggle LED
static void led_toggle(void) {
	led_set(!led_get());
}

// LED is toggling after a number of calls
static void led_blinking(void)
{
	static uint32_t update_tick = 0;
	const uint32_t interval = CLOCK_TICKS_PER_SECOND / 2;

	// if last update is back more than an interval
	if (update_tick + interval < clock_get_loop_tick())
	{
		led_toggle();
		update_tick = clock_get_loop_tick();
	}
}

// initial config for using LED
void led_init(void) {
	PIOB->PIO_PER |= PIO_PER_P27;
	PIOB->PIO_OER |= PIO_OER_P27;

	PMC->PMC_PCER0 |= 1 << ID_PIOD;
	
	PIOD->PIO_ODR |= PIO_ODR_P0 | PIO_ODR_P1 | PIO_ODR_P2;
}

// loop for LED control - blinking when EDA code is running
void led_loop(void)
{
	if (state.led == LED_NOLOOP)
		return;

	uint32_t input_data_status = PIOD->PIO_PDSR;

	if((input_data_status & PIO_PDSR_P0) == 0) {
		state.led = LED_ON;
	} else if((input_data_status & PIO_PDSR_P1) == 0) {
		state.led = LED_OFF;
	} else if((input_data_status & PIO_PDSR_P2) == 0) {
		state.led = LED_BLINKING;
	}

	switch (state.led)
	{
	case LED_BLINKING:
		led_blinking();
		break;
	case LED_ON:
		led_set(true);
		break;
	case LED_OFF:
		led_set(false);
		break;
	case LED_NOLOOP:
	default:
		break;
	}
}
