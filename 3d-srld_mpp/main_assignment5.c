#include "main.h"
#include "aufgabe.h"
#include <stdint.h>

// global variable
int32_t timer = 0;
int counter;
char counter_char;

int main(void)
{

    // Assignment 5
    // Window Watchdog

    unsigned char value_watchdog_counter = 0x7f;
    unsigned char window_value = 0x50;
    unsigned char window_value_refresh = 0x50;
    unsigned char cnt_i = 0;
    unsigned char cnt_j = 0;

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

    // assignment 5: Independent Watchdog
    //init_iwdg();

    // Assignment 5
    // Window Watchdog

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
    WWDG_SetPrescaler(WWDG_Prescaler_8);
    WWDG_SetWindowValue(window_value);
    WWDG_Enable(value_watchdog_counter);

    init_button_2();

    /* Start der Real Time Clock
    if RTC was not started, will be initialize with
     low-speed external 32.768 kHz oscillator (LSE)
    */
    start_RTC();
    counter=0;
    char usart2_tx_buffer[50];

    //cnt_i = (unsigned char) (value_watchdog_counter + 1);


    while(1){

        cnt_j = (unsigned char) ((WWDG->CR) & 0x7F) ;

        if (cnt_j  < cnt_i ) {

            sprintf(usart2_tx_buffer,"i = %u\r\n",cnt_j);
            usart2_send_text(usart2_tx_buffer);

            cnt_i = cnt_j;

            if (cnt_i == window_value_refresh ) {

                WWDG_SetCounter(value_watchdog_counter);

                sprintf(usart2_tx_buffer,"####### neu geladen\r\n");
                usart2_send_text(usart2_tx_buffer);

                cnt_i = (unsigned char) (value_watchdog_counter + 1);
            }
        }

        /* Assignment 5 Independent Watchdog
        counter++;
        sprintf(tx, "Schleife: %d\r\n", counter);
        usart2_send_text(tx);
        wait_uSek(500000);

        IWDG_ReloadCounter();
        if (GPIO_ReadInputDataBit ( GPIOC , GPIO_Pin_5 ) != 0) {
            usart2_print("Taste2 gedrückt\r\n");
            wait_uSek(5000000);
        }
        */
    }
    return 0; // to make the warning stop
}
