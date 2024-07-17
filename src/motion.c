// Eingebettete Systeme  / Embedded Systems
// Embedded Distance Assistent (EDA)
// SEMESTER: SS24
// AUTHORS: David Heiss, Manfred Pester, Jens-Peter Akelbein
// FILE: motion.c
// CONTENTS: implementation for all functions of motion module

#include <motion.h>
#include <sample.h>
#include <state.h>

#define KP 0.01
#define TV 0.1

#define DUTY_CYCLE_MAX 1
#define TOLERANCE 10 // tolerance in mm

static float last_error = 0;
static float current_error = 0;
static float derivative = 0;

static float control_signal = 0;

// current movement is zero mm/sec
int32_t motion_current = 0;

void motion_init(void)
{
    // todo
    state.motion = MOTION_ON;
    duty_cycle = 0.5;
}


void determine_motion_direction()
{
    if (current_error > TOLERANCE) {
        // Need to move forward
        state.motion = MOTION_ON;
        direction = true;
    } else if (current_error < -TOLERANCE){
        // Need to move backward
        state.motion = MOTION_ON;
        direction = false;
        control_signal = -control_signal; // Reverse control signal for backward direction
    } else {
        state.motion = MOTION_OFF;
        control_signal = 0;
    }
}

void limit_motion_intensity_if_necessary()
{
    if (control_signal > DUTY_CYCLE_MAX) {
        control_signal = DUTY_CYCLE_MAX;
    }
}


void motion_loop(void)
{
    struct sample_t current_sample = sample_series_get(distance_raw, 0);

    float distance = current_sample.value / 1000.0;
    current_error = gap_target - distance;

    derivative = (current_error - last_error);

    control_signal = (KP * current_error) +  (TV * derivative);

    determine_motion_direction();

    limit_motion_intensity_if_necessary();

    last_error = current_error;
    duty_cycle = control_signal;
}
