#include "main.h"
#include "aufgabe.h"
#include <stdint.h>

// global variable
int32_t timer = 0;
int counter;
char counter_char;

int main(void)
{

    // Initialization of the system and the clock system
    SystemInit();

    // Initialize SysTick
    // The call then takes place every ms
    // of the handler for the SysTick_IRQn interrupt
    InitSysTick();

    // Initialization of all port lines and interfaces
    // Release of interrupts
    our_init_board();

    //beep(4000,200,0); // test beep

    // assignment 2
    //init_leds(RCC_AHB1Periph_GPIOB, GPIOB, GPIO_Pin_2);
    //init_button_1();
    //init_button_2();

    // assignment 5: Watchdog
    init_button_2();
    init_iwdg();

    /* Start der Real Time Clock
    if RTC was not started, will be initialize with
     low-speed external 32.768 kHz oscillator (LSE)
    */
    start_RTC();
    counter=0;
    char tx[50];

    while(1){
        counter++;
        sprintf(tx, "Schleife: %d\r\n", counter);
        usart2_send_text(tx);
        wait_uSek(500000);

        IWDG_ReloadCounter();
        if (GPIO_ReadInputDataBit ( GPIOC , GPIO_Pin_5 ) != 0) {
            usart2_print("Taste2 gedrückt\r\n");
            wait_uSek(5000000);
        }
        //counter_char = counter+'0'; // convert int to char by building ascci value of char. 1+'0'=='1'
        // Assignment 4 task 2.3.
        //usart2_send_text(&counter_char);
        // Assignment 4, task 2.5
        //usart2_print(": Cora und Oleksandra, Frohe Weihnachten!!");
        //usart2_send_text("\r\n");
        //counter = (counter +1) % 10;

        // for assignment 4, task 2.6 the previous lines
        // should be commented to free uart
        // from looped output

        // We set NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
        // and implement everything in IRQ_Handler

        //timer = 1000; // 1 second
        //while (timer) {;}
    }
    return 0; // to make the warning stop
	}
