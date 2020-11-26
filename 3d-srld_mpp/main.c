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

    /* Start der Real Time Clock
    if RTC was not started, will be initialize with
     low-speed external 32.768 kHz oscillator (LSE)
    */
    start_RTC();
    //init_leds(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_Pin_2);

    while(1)
		{

    	uwbranging_tick();
    	wait_mSek(500);

    	// Abstände zu den 4 Ankerknoten werden in einer Endlosschleife
    	// an den MQTT Broker geschickt
    	//lokalisieren();

    	// Abstände zu den 4 Ankerknoten werden einmalig bestimmt
    	// und auf der Seriellen ausgegeben
    	position();

		}
	}
