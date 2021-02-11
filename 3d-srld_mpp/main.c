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
char USART2_TX_BUF[USART2_BUFFERSIZE];
char USART2_RX_BUF[USART2_BUFFERSIZE];


int main(void)
{
    SystemInit();
    init_board();
    start_RTC();
    InitSysTick();
    CoInitOS();
    create_tasks();
    CoStartOS();
    
    while(1){;}
    return 0;
}
