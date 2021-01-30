#include "main.h"
#include "aufgabe.h"
#include <stdint.h>

// global variable
int32_t timer = 0;
int counter;
char counter_char;
int timer_interrupt_count = 0;
int used_timer = 0;
int timer_runs = 0;

int main(void)
{
    SystemInit();
    InitSysTick();
    start_RTC();

    init_leds(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_Pin_2);
    init_button_1_irq();
    init_button_2_irq();

    our_init_board();
    
    // assignment 9 task 2.2
    init_timer_7();
    


    while(1){;}
    return 0; // to make the warning stop
}
