#include "aufgabe.h"
#include "main.h"
#include <string.h>

GPIO_InitTypeDef GPIO_InitStructure;
char usart2_tx_buffer[USART2_TX_BUFFERSIZE_50];
char usart2_rx_buffer[USART2_RX_BUFFERSIZE_50];
unsigned char usart2_busy = 0;


/* Init the GPIO as Output Push Pull with Pull-up
 * on selected Port with selected Pin
 * set output pin to zero (initial state is off)
 */
void init_leds(uint32_t RCC_AHB1Periph, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph,ENABLE );
    GPIO_StructInit (&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode =   GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;


    // GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct
    GPIO_Init(GPIOx, &GPIO_InitStructure );
    // turn off LED
    GPIO_ResetBits(GPIOx, GPIO_InitStructure.GPIO_Pin);
}

void init_button(uint16_t GPIO_Pin)
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE );
    RCC_AHB1PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE );

    GPIO_InitTypeDef GPIO_ButtonInitStructure;
    GPIO_StructInit (&GPIO_ButtonInitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode =   GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //we do not need anything faster as people are not so fast to press a button
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_OType =  GPIO_OType_PP;

    GPIO_Init(GPIOC, &GPIO_ButtonInitStructure);

}

void init_button_1()
{
    init_button(GPIO_Pin_8); // PC8
}

void init_button_2()
{
    init_button(GPIO_Pin_5); // PC5
}

void led_on_off()
{
    uint8_t count_button_2 = 0;
    while(1)
	{
        uwbranging_tick();
        if (GPIO_ReadInputDataBit ( GPIOB, GPIO_Pin_2) == 1) { // check only for button 1 if led is on
            if (GPIO_ReadInputDataBit ( GPIOC , GPIO_Pin_8 ) != 1) {
                GR_LED_OFF;
                count_button_2 = 0; // button 2 has to be pressed twice again to turn led on
            }
        }
        if (GPIO_ReadInputDataBit ( GPIOC , GPIO_Pin_5 ) != 0) {
            count_button_2 += 1;
            wait_uSek(500000); // if we not wait here a while one button press of a human will be registered as multiple ones
        }
        if (count_button_2 >= 2) {
            GR_LED_ON;
            count_button_2 = 0;
        }
    	wait_uSek(50000);

    }
}

void init_usart_2_tx() {
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// activate clock system
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// configure GPIOA:  USART2 TX on PA2 RX on PA3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// Alternative Function ion GPIO
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);

	// USART init
	USART_InitStructure.USART_BaudRate = 921600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	// USART deactivate
	USART_Cmd(USART2, ENABLE);
}

void usart2_send_text(char *chars)
{
    int i = 0;
    for ( i = 0; i < strlen ( chars ); i ++)
    {
        USART_SendData(USART2, chars [ i ]);

    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    }
}

/* generates a character string in an endless loop with a running index,
     * a text and the control character sequence '\ r \ n'
     * at the end of the character string.
     * */
void usart2_2_print(char *chars)
{
    char msg[strlen(chars) + 5];
    if (chars)
    {
        int length = strlen(msg);
        if (length <= USART2_TX_BUFFERSIZE_50)
        {
            // Wait for the last transfer to complete
            //while (usart2_busy && DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) == RESET) asm("");
            //DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);

            // The USART is active
            usart2_busy = 1;

            // Copy the string into the TX buffer
            strcpy(usart2_tx_buffer, chars);

            // Enter the package length (nested so that only one calculation is necessary)
            DMA_SetCurrDataCounter(DMA1_Stream6, (unsigned short)length);

            // Activate the DMA transfer
            DMA_Cmd(DMA1_Stream6, ENABLE);
        }
    }
}

void our_init_board(){
    init_POWER_ON();

    init_usart_2_tx();
	usart2_send_text("____Start____\r\n");

    init_BEEPER();
    usart2_send_text("=> BEEPER\r\n");
    usart2_send_text("_____________\r\n");
}