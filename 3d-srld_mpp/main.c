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
    start_RTC_Alarm();
    // Sheet 7 Exercise 3.3
    // only execute one of the set_RTC_Alarm functions
    //set_RTC_Alarm_Mondays();
    set_RTC_Alarm_Thirds();

    while(1){
        // pause 1 sec
        wait_mSek(1000);
        get_sys_time();
    }
    return 0; // to make the warning stop
}
