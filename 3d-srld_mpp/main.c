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
    start_RTC();

    // Assignment 10
    // Part DMA
    /* for the Assignment 10 - DMA we will use USART Init Function and DMA Init function */
    init_USART2_TX();
    init_USART2_RX_IRQ();
    usart2_send("\r\nStarted UART-DMA\r\n");

    init_DMA1_Stream6();

    while(1){

    }
    return 0;
}
