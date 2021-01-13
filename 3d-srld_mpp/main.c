#include "main.h"
#include "aufgabe.h"
#include <stdint.h>

// global variable
int32_t timer = 0;
int counter;
char counter_char;

int main(void)
{
    // ==================================================
    // BASE CALLS THAT WE USE IN EACH ASSIGNMENT

    // Initialization of the system and the clock system
    SystemInit();

    /* Start der Real Time Clock */
    start_RTC();

    // Initialize SysTick
    // The call then takes place every ms of the handler for the SysTick_IRQn interrupt
    InitSysTick();


    our_init_board();


    while(1){
        // The core of the sample application should consist of an empty endless loop.

        //timer = 1000; // 1 second
        //while (timer) {;}
    }
    return 0; // to make the warning stop
}
