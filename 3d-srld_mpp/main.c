#include "main.h"
#include "aufgabe.h"
#include <stdint.h>

// global variable
int32_t timer = 0;
int counter;
char counter_char;

int main(void)
{

    // Initialisierung des Systems und des Clocksystems
    SystemInit();

    // SysTick initialisieren
    // jede ms erfolgt dann der Aufruf
    // des Handlers fuer den Interrupt SysTick_IRQn
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
    counter=0;

    while(1){

        // We set NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        // and implement everything in IRQ_Handler

        wait_mSek(1000);
        get_sys_time();

    }
    return 0; // to make the warning stop
	}
