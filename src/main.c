// Eingebettete Systeme  / Embedded Systems
// Embedded Distance Assistent (EDA)
// SEMESTER: SS24
// AUTHORS: 
// FILE: matrix.c
// CONTENTS: main loop 

#include <serial.h>
#include <menu.h>
#include <clock.h>
#include <string.h>
#include <led.h>
#include <twi.h>
#include <matrix.h>
#include <sonic.h>
#include <sample.h>
#include <drive.h>
#include <motion.h>

void init(void)
{
    serial_init();
    menu_init();
    clock_init();
    led_init();

    // twi-initialization
    Twi *const twi = TWI1;
    const uint32_t frequency = 3000;
    twi_init(twi, frequency, false);

    matrix_init();
    sonic_init();
    motion_init();
    drive_init();
}

void loop(void)
{
    clock_loop();
    menu_loop();
    led_loop();
    matrix_loop();
    drive_loop();
    // sonic_loop(); -> implemented via interrupt
    sample_loop();
    motion_loop();
}

int main(void)
{
    init();
    while (1) {
        loop();
    }
}
