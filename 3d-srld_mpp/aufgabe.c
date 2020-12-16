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

void init_usart_2_tx_rx() {
    // Initialisierungsstrukturen vorbereiten
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // Clocksystem aktivieren
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    // GPIOA Configuration:  USART2 TX on PA2 RX on PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Alternative Funktion festlegen
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // USART Initialisieren
    USART_InitStructure.USART_BaudRate = 921600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);

    // Deinitialisiere die DMA
    DMA_DeInit(DMA1_Stream6); //USART2_TX_DMA_STREAM

    // Structure auf default setzen
    DMA_StructInit(&DMA_InitStructure);

    // Deaktiviere den DMA Transfer
    DMA_Cmd(DMA1_Stream6, DISABLE);

    //// Lösche das DMA-Flag
    DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);

    // Lege die Grundinitialisierung fest
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable ;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull ;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single ;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->DR));
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)usart2_tx_buffer;
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);

    // Fordere RX und TX an
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    // USART Deaktivieren
    USART_Cmd(USART2, ENABLE);

    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_EnableIRQ(USART2_IRQn);

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

void usart2_print(char *chars)
{
    // DRY: do not repeat yourself
    usart2_send_text(chars);
}

void our_init_board(){
    init_POWER_ON();

    //init_usart_2_tx();
    init_usart_2_tx_rx();

    usart2_send_text("____Start____\r\n");
    usart2_send_text("=> UART RX/TX \r\n");
    usart2_send_text("_____________\r\n");

    init_BEEPER();
    usart2_send_text("=> BEEPER\r\n");
    usart2_send_text("_____________\r\n");
}

void usart2_get_char() {

    /* Receive Data */
    char rx_led[1024];
    long int raw_input;

    //strcpy(rx_led, USART_ReceiveData(USART2));
    usart2_print(usart2_rx_buffer);
    usart2_send_text("\r\n");



    //int number = (int)rx_led[0] - 48;
    //usart2_print("Got:");
    //usart2_print(rx_led);
    //usart2_send_text("\r\n");


    //if (number == 1) {
    //    usart2_print("grüne LED im 1 Sekundentakt");
    //    usart2_send_text("\r\n");
    //}
}