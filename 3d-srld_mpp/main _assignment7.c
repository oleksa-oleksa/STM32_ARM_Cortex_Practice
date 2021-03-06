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

    // Initialize SysTick
    // The call then takes place every ms of the handler for the SysTick_IRQn interrupt
    InitSysTick();

    // Assignment 6: Interrupts
    // In the initial situation, the green LED should be switched off.
    init_leds(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_Pin_2);
    // Both buttons on PC8 and PC5 should be interrupt capable
    //init_button_1_irq();
    //init_button_2_irq();

    our_init_board();

    /* Start der Real Time Clock
    if RTC was not started, will be initialize with
     low-speed external 32.768 kHz oscillator (LSE)
    */
    start_RTC();
    start_RTC_Alarm();

    // Sheet 7 Exercise 3
    // only execute one of the set_RTC_Alarm functions
    //set_RTC_Alarm_Mondays();
    //set_RTC_Alarm_Thirds();
    //set_RTC_Alarm_each_25_secs();

    while(1){

        // We set NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        // and implement everything in IRQ_Handler

        //wait_mSek(1000);
        //get_sys_time();

    }
    return 0; // to make the warning stop
}
