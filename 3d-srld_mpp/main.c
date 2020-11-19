#include "main.h"

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

    // Start der RTC  falls diese noch
    // nicht initialisiert war wird
    // die RTC mit der LSE-Taktquelle aktiviert
    start_RTC();

	// Start des WLAN Transceivers im Station Mode
    // Anmeldung an der Fritzbox
    CC3100_set_in_STA_Mode(0);

    // Initialisierung des CoCox Real Time Betriebssystems CoOS
    //CoInitOS ();

    // Anmelden notwendiger applikationsspezifischer Task
    //CoCreateTask (...);

    // Start des Betriebssystems CoOS
    //CoStartOS ();

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
