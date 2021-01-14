#include "aufgabe.h"
#include "main.h"
#include <string.h>

GPIO_InitTypeDef GPIO_InitStructure;
char usart2_tx_buffer[USART2_TX_BUFFERSIZE_50];
char usart2_rx_buffer[USART2_RX_BUFFERSIZE_50];
unsigned char usart2_busy = 0;
int led_timer = 1000;

char date_buf[5];

// sudo chmod 0777 /dev/ttyUSB0

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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
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

void init_iwdg() { // configure to count 5 secs 
    // activate write permissions
    IWDG_WriteAccessCmd ( IWDG_WriteAccess_Enable );
    // set prescaler (4 , 8 , 16 ,... , 256)
    IWDG_SetPrescaler ( IWDG_Prescaler_64 );
    // set value from which it counts down (0...4095)
    IWDG_SetReload (2500);
    // set wachdog to the max value (2500) 
    IWDG_ReloadCounter ();
    // activate IWDG
    IWDG_Enable ();
}

void our_init_board(){
    init_POWER_ON();

    //init_usart_2_tx();
    init_usart_2_tx_rx();

    usart2_send_text("\r\nNeustart\r\n");
    usart2_send_text("=> UART RX/TX \r\n");
    usart2_send_text("_____________\r\n");

    //init_iwdg();
    //usart2_send_text("=> IWDG \r\n");


    //init_BEEPER();
    //usart2_send_text("=> BEEPER OFF (!) \r\n");
    //usart2_send_text("_____________\r\n");
}

void USART2_IRQ_LED_CONTROL(void)
{
    char c;
    static int j = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        c = (char)USART_ReceiveData(USART2);
        if (c=='\r')	// End of string input
        {
            usart2_rx_buffer[j] = 0x00 ;

            if (usart2_rx_buffer[0] == '1') {
                strcpy(usart2_tx_buffer, "grüne LED im 1 Sekundentakt\r\n");
                led_timer = 1000;
            }

            else if (usart2_rx_buffer[0] == '2') {
                strcpy(usart2_tx_buffer, "grüne LED im 2 Sekundentakt\r\n");
                led_timer = 2000;
            }

            else if (usart2_rx_buffer[0] == '4') {
                strcpy(usart2_tx_buffer, "grüne LED im 4 Sekundentakt\r\n");
                led_timer = 4000;
            }

            else {
                //strcpy(usart2_tx_buffer, "Nur 1, 2 oder 4 sind erwartet!\r\n");
                // Assignment 4, task 2.7
                sprintf(usart2_tx_buffer, "  Zeichenkette=%s Länge=%d\r\n", usart2_rx_buffer, j);
            }

            usart2_send(usart2_tx_buffer);
            memset(usart2_rx_buffer,0x00,20);
            j=0;
        }
        else
        {
            usart2_rx_buffer[j] = c;
            j++;
            if (j >= 30) { j = 0; }
        }
    }
}

void USART2_IRQ_LED_CONTROL_WITH_OFF() {
    char c;
    static int j = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        c = (char)USART_ReceiveData(USART2);
        if (c=='\r')	// End of string input
        {
            usart2_rx_buffer[j] = 0x00 ;

            if (usart2_rx_buffer[0] == '1') {
                strcpy(usart2_tx_buffer, "grüne LED im 1 Sekundentakt\r\n");
                led_timer = 1000;
            }

            else if (usart2_rx_buffer[0] == '4') {
                strcpy(usart2_tx_buffer, "grüne LED im 4 Sekundentakt\r\n");
                led_timer = 4000;
            }

            else if (usart2_rx_buffer[0] == 's') {
                strcpy(usart2_tx_buffer, "grüne LED ist AUS\r\n");
                LED_GR_OFF;
                led_timer = 0;
            }

            // case: set Time and Date, LED will be turned off for a silence purpose
            else if (usart2_rx_buffer[0] == 'd') {
                strcpy(usart2_tx_buffer, "Enter date!\r\n");
                LED_GR_OFF;
                led_timer = 0;
            }

            else {
                //strcpy(usart2_tx_buffer, "Nur 1, 2 oder 4 sind erwartet!\r\n");
                // Assignment 4, task 2.7
                sprintf(usart2_tx_buffer, "  Zeichenkette=%s Länge=%d\r\n", usart2_rx_buffer, j);
            }

            usart2_send(usart2_tx_buffer);
            memset(usart2_rx_buffer,0x00,20);
            j=0;
        }
        else
        {
            usart2_rx_buffer[j] = c;
            j++;
            if (j >= 30) { j = 0; }
        }
    }
}


void init_button_1_irq() {
    // pressed - 0, not pressed - 1: LOW ACTIVE -> HL for trigger
    // PC8 button1 delivers an interrupt on a HL edge
    // for initial GPIO initialization the previously created function from assignment 2 is used
    init_button_1();

    /* Set variables used for IRQ */
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Use PC8 for EXTI_Line8 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);

    /* PC8 is connected to EXTI_Line8 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line8;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on HL High -> Low falling edge
     * A change of state, i.e. an edge, serves as a start or stop condition.*/
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    // EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

    /* Add IRQ vector to NVIC */
    /* PC8 is connected to EXTI9_5_IRQn
     * the port lines 5-9 and 10-15 are bundled on the EXTI9-5_IRQn and EXTI15-10_IRQn */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    /* Set priority: This parameter can be a value between 0 and 15 */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}

void init_button_2_irq() {
    // pressed - 1, not pressed - 0: LOW ACTIVE -> LH for trigger ___|--
    // Active-HIGH button means that when you press/close the switch, then the signal sent to the MCU will be HIGH.
    // PC8 button1 delivers an interrupt on a LH edge
    // for initial GPIO initialization the previously created function from assignment 2 is used
    init_button_2();

    /* Set variables used for IRQ */
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Use PC5 for EXTI_Line5 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource5);

    /* PC5 is connected to EXTI_Line5 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line5;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* A change of state, i.e. an edge, serves as a start or stop condition.*/
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    // EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

    /* Add IRQ vector to NVIC */
    /* PC5 is connected to EXTI9_5_IRQn
     * the port lines 5-9 and 10-15 are bundled on the EXTI9-5_IRQn and EXTI15-10_IRQ */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    /* Set priority: This parameter can be a value between 0 and 15 */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}

void button_1_handler() {
    GR_LED_ON;
    usart2_send("Green LED is ON\r\n");
}
void button_2_handler() {
    GR_LED_OFF;
    usart2_send("Green LED is OFF\r\n");
}

void deinit_button_1_irq() {
    /* Set variables used for IRQ */
    EXTI_InitTypeDef EXTI_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Use PC8 for EXTI_Line8 */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource8);

    /* PC8 is connected to EXTI_Line8 */
    EXTI_InitStruct.EXTI_Line = EXTI_Line8;
    /* Enable interrupt */
    EXTI_InitStruct.EXTI_LineCmd = DISABLE;
    /* Interrupt mode */
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    /* Triggers on HL High -> Low falling edge
     * A change of state, i.e. an edge, serves as a start or stop condition.*/
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    // EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
    /* Add to EXTI */
    EXTI_Init(&EXTI_InitStruct);

    /* Add IRQ vector to NVIC */
    /* PC8 is connected to EXTI9_5_IRQn
     * the port lines 5-9 and 10-15 are bundled on the EXTI9-5_IRQn and EXTI15-10_IRQn */
    NVIC_InitStruct.NVIC_IRQChannel = EXTI9_5_IRQn;
    /* Set priority: This parameter can be a value between 0 and 15 */
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
    /* Enable interrupt */
    NVIC_InitStruct.NVIC_IRQChannelCmd = DISABLE;
    /* Add to NVIC */
    NVIC_Init(&NVIC_InitStruct);
}

int bcd_decimal(uint8_t hex)
{
    // More significant nybble is valid
    if (((hex & 0xF0) >> 4) < 10) {
        // Less significant nybble is valid
        if ((hex & 0x0F) < 10);
        {
            int dec = ((hex & 0xF0) >> 4) * 10 + (hex & 0x0F);
            return dec;
        }
    }
    else {
        return hex;
    }
}

void usart2_send_time(RTC_TimeTypeDef time) {
    usart2_send("Time: ");
    int hours = bcd_decimal(time.RTC_Hours);
    sprintf(date_buf, "%i", hours);
    usart2_send(date_buf);
    usart2_send(":");
    int minutes = bcd_decimal(time.RTC_Minutes);
    sprintf(date_buf, "%i", minutes);
    usart2_send(date_buf);
    usart2_send(":");
    int seconds = bcd_decimal(time.RTC_Seconds);
    sprintf(date_buf, "%i", seconds);
    usart2_send(date_buf);
    usart2_send("\r\n");
}

void usart2_send_date(RTC_DateTypeDef date) {
    usart2_send("Day: ");
    int day = bcd_decimal(date.RTC_Date);
    sprintf(date_buf, "%i", day);
    usart2_send(date_buf);
    usart2_send(".");
    int month = bcd_decimal(date.RTC_Month);
    sprintf(date_buf, "%i", month);
    usart2_send(date_buf);
    usart2_send(".");
    int year = bcd_decimal(date.RTC_Year);
    sprintf(date_buf, "%i", year);
    usart2_send(date_buf);
    usart2_send("\r\n");
}

void get_sys_time() {
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;
    uint8_t buffer[20];

    // FORMAT is RTC_Format_BIN) || RTC_Format_BCD
    // With these functions we copy data from TRC registers to our two variables (sTime and sDate).
    RTC_GetTime(RTC_Format_BCD, &sTime);
    RTC_GetDate(RTC_Format_BCD, &sDate);

    // The data is BCD coded so we need to
    // convert a binary-coded decimal number into a decimal number in terms of representation
    usart2_send_time(sTime);
    usart2_send_date(sDate);
}