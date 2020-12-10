#include "main.h"
#include "aufgabe.h"
#include <stdint.h>

// global variable
int32_t timer = 0;

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


    while(1){
        // led_on_off();

        timer = 500;
        while (timer) {;}
        LED_GR_OFF;

        timer = 3000; // 3 seconds
        while (timer) {;}
        LED_GR_ON;
    }
    return 0; // to make the warning stop
	}
