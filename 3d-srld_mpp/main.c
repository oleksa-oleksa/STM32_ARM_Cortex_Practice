#include "main.h"
#include "aufgabe.h"
#include <stdint.h>

// global variable
int32_t timer = 0;
int counter;
char counter_char;

int main(void)
{
    SystemInit();
    //InitSysTick();

    // Assignment 8: Sleep Mode
    init_leds(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_Pin_2);
    init_button_1();
    init_button_2_irq();

    our_init_board();

    start_RTC();

    // assignment 8
    //sleep_mode_test();
    stop_mode_test();

    while(1){

        }
    return 0; // to make the warning stop
}
