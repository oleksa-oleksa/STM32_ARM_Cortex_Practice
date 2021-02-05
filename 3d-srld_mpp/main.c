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

    // Assignment 10
    // Part DMA
    init_USART2_TX_DMA();
    //init_DMA1_Stream6();
    usart2_send("\r\nNeustart UART-DMA\r\n");

    while(1){

    }
    return 0;
}
