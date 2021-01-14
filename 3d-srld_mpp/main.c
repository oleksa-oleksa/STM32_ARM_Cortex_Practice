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
    // The call then takes place every ms
    // of the handler for the SysTick_IRQn interrupt
    InitSysTick();

    // Initialization of all port lines and interfaces
    // Release of interrupts
    our_init_board();

    //beep(4000,200,0); // test beep

    // ==================================================

    // Assignment 2: GPIO Initialisation and LED control
    //init_leds(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_Pin_2);
    //init_button_1();
    //init_button_2();

    // ==================================================

    // Assignment 5: Independent Watchdog
    //init_iwdg();

    // ==================================================

    // Assignment 5
    // Window Watchdog
    /*
    unsigned char value_watchdog_counter = 0x7f;
    unsigned char window_value = 0x50;
    unsigned char window_value_refresh = 0x50;
    unsigned char cnt_i = 0;
    unsigned char cnt_j = 0;
    */

    // Assignment 5
    // Window Watchdog
    /*
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    WWDG_SetWindowValue(window_value);
    WWDG_Enable(value_watchdog_counter);
    */

    // ==================================================
    // Assignment 6: Interrupts
    // In the initial situation, the green LED should be switched off.
    init_leds(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_Pin_2);
    // Both buttons on PC8 and PC5 should be interrupt capable
    init_button_1_irq();
    init_button_2_irq();

    /* Start der Real Time Clock
    if RTC was not started, will be initialize with
     low-speed external 32.768 kHz oscillator (LSE)
    */
    start_RTC();
    char usart2_tx_buffer[50];

    //cnt_i = (unsigned char) (value_watchdog_counter + 1);


    while(1){
        // The core of the sample application should consist of an empty endless loop.

        wait_mSek(1000);
        get_sys_time();
    }
    return 0; // to make the warning stop
}
