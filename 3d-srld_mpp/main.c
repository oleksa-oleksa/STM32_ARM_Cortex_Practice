#include "main.h"
#include "aufgabe.h"

int main(void)
{

    // Initialisierung des Systems und des Clocksystems
    SystemInit();

    // SysTick initialisieren
    // jede ms erfolgt dann der Aufruf
    // des Handlers fuer den Interrupt SysTick_IRQn
    InitSysTick();

    // Initialisierung aller Portleitungen und Schnittstellen
    // Freigabe von Interrupten
    init_board();

    // assignment 2
    init_leds(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_Pin_2);
    init_button_1();
    init_button_2();
    
    /* Start der Real Time Clock
    if RTC was not started, will be initialize with
     low-speed external 32.768 kHz oscillator (LSE)
    */
    start_RTC();

    // Assignment 2, task 3
    led_on_off();

    /* Assignment 2, task 2
    while(1) {
        GR_LED_ON;
        wait_uSek(1000000);
        GR_LED_OFF;
        wait_uSek(1000000);
     }
     */

    return 0; // to make the warning stop
	}
