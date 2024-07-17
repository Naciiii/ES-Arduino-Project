// motion.c

#include <sam3x8e.h>
#include <drive.h>
#include <state.h>
#include <math.h>
#include <stdio.h>

#define DRIVE_DUTY_FRQ 100
void drive_init(void)
{
     // Enable the peripheral clock for the timer counter
    PMC->PMC_PCER1 |= 1 << ID_TC6 % 32;  // Timer Counter 2 Channel 0

    // Configure PIO_PC23 and PIO_PC24 as output
    PIOC->PIO_OER = PIO_PC23 | PIO_PC24;
    PIOC->PIO_PER = PIO_PC23 | PIO_PC24;
    PIOC->PIO_CODR = PIO_PC23 | PIO_PC24;  // Ensure both are low initially

    // Enable peripheral control of the PIO_PC25 for PWM
    PIOC->PIO_PDR = PIO_PC25;
    PIOC->PIO_ABSR |= PIO_PC25B_TIOA6;  // Select Peripheral A function for PIOC25
    
    // Disable the timer counter
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKDIS;

    // Configure the timer counter for waveform mode
    TC2->TC_CHANNEL[0].TC_CMR = TC_CMR_WAVE                // Waveform mode
                                |TC_CMR_TCCLKS_TIMER_CLOCK1  // Clock selection
                                | TC_CMR_WAVSEL_UP_RC        // UP mode with automatic trigger on RC compare
                                | TC_CMR_ACPA_CLEAR            // RA compare effect on TIOA
                                | TC_CMR_ACPC_SET;         // RC compare effect on TIOA

    uint32_t rc = DUE_MCK2 / DRIVE_DUTY_FRQ;  // MCK/2 due to TIMER_CLOCK1
    uint32_t ra = (uint32_t)(rc * duty_cycle);

    // Update the RA and RC values to adjust the duty cycle
    TC2->TC_CHANNEL[0].TC_RA = ra;
    TC2->TC_CHANNEL[0].TC_RC = rc;

    // Enable the counter and start the timer
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;
}

void drive_loop(void)
{

    if (state.motion == MOTION_ON)
    {
        if (direction)
        {
            // Forward direction
            PIOC->PIO_SODR = PIO_PC23;
            PIOC->PIO_CODR = PIO_PC24;
        }
        else
        {
            // Reverse direction
            PIOC->PIO_SODR = PIO_PC24;
            PIOC->PIO_CODR = PIO_PC23;
        }


        // Adjust PWM duty cycle
         drive_duty(DRIVE_DUTY_FRQ, duty_cycle);
    }
    else
    {
        // Stop the motor
        PIOC->PIO_CODR = PIO_PC23 | PIO_PC24;
        drive_duty(DRIVE_DUTY_FRQ, 0); // Set duty cycle to 0 to stop PWM
    }
}

void drive_duty(const uint32_t freq, const float duty)
{
    static float duty_before = -1;
    if(duty == duty_before) {
        return;
    }
    uint32_t rc = DUE_MCK2 / freq;  // MCK/2 due to TIMER_CLOCK1
    uint32_t ra = (uint32_t)(rc * duty);

    // Update the RA and RC values to adjust the duty cycle
    TC2->TC_CHANNEL[0].TC_RA = ra;
    TC2->TC_CHANNEL[0].TC_RC = rc;

    // Trigger a software reset and enable the clock
    TC2->TC_CHANNEL[0].TC_CCR = TC_CCR_CLKEN | TC_CCR_SWTRG;

    duty_before = duty;
}

