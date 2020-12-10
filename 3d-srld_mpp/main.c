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

    // Initialisierung aller Portleitungen und Schnittstellen
    // Freigabe von Interrupten
    our_init_board();

    beep(4000,200,0); // test beep

    // assignment 2
    init_leds(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_Pin_2);
    init_button_1();
    init_button_2();
    
    /* Start der Real Time Clock
    if RTC was not started, will be initialize with
     low-speed external 32.768 kHz oscillator (LSE)
    */
    //start_RTC();
    counter=0;

    while(1){
        counter_char = counter+'0'; // convert int to char by building ascci value of char. 1+'0'=='1'
        // Assignment 4 task 2.3.
        usart2_send_text(&counter_char);
        // Assignment 4, task 2.5
        usart2_2_print(": Cora und Oleksandra, Frohe Weihnachten!!");
        usart2_send_text("\r\n");

        counter = (counter +1) % 10;

        timer = 1000; // 1 second
        while (timer) {;}
    }
    return 0; // to make the warning stop
	}
