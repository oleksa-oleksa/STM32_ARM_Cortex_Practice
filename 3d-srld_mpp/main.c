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
int reflex_test_round = 0;
int reflex_test_runs = 0;
int reflex_round_active = 0;

uint32_t tim3_counter;
char buffer[60];

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
    // assignment 9 task 2.3
    //tim3_monitor_button_1_usage();


    while(1){
        // assignment 9 task 2.3
        /*
        tim3_counter = TIM_GetCounter(TIM3);
		sprintf(buffer, "Button 1 was pressed %u times\r\n", tim3_counter);    
        usart2_send(buffer);
        wait_mSek(2000);
        */
    }
    return 0; // to make the warning stop
}
