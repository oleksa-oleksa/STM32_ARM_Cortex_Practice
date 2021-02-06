#include "aufgabe.h"
#include "main.h"
#include <string.h>

GPIO_InitTypeDef GPIO_InitStructure;
char usart2_tx_buffer[USART2_TX_BUFFERSIZE_50];
char usart2_rx_buffer[USART2_RX_BUFFERSIZE_50];
char dma_usart2_rx[255];
char dma_usart2_tx[255];
unsigned char usart2_busy = 0;
int led_timer = 1000;
char date_buf[5];
int date_flag = 0;
int time_flag = 0;
int dt_flag = 0;
int led_flag = 1;
int alarm_type = 0;
unsigned char my_usart2_running = 0;
char USART2_TX_BUF[USART2_BUFFERSIZE];
char USART2_RX_BUF[USART2_BUFFERSIZE];

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
    //init_POWER_ON();

    init_usart_2_tx_rx();
    //init_button_1();
    //init_button_2();
    
    usart2_send("\r\nNeustart\r\n");

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
            memset(usart2_rx_buffer,0x00,USART2_RX_BUFFERSIZE_50);
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


void USART2_GET_DATATIME(void)
{
    char c;
    static int j = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        c = (char)USART_ReceiveData(USART2);
        if (c=='\r')	// End of string input
        {
            usart2_rx_buffer[j] = 0x00 ;

            if (!dt_flag) {
                strcpy(usart2_tx_buffer, "Only d (date) und t (time) are expected!\r\n");
                // Assignment 4, task 2.7
                //sprintf(usart2_tx_buffer, "  Zeichenkette=%s Länge=%d\r\n", usart2_rx_buffer, j);
            }

            if (date_flag) {
                parse_date(usart2_rx_buffer);
                usart2_send("\r\n");
                // memset(usart2_rx_buffer,0x00,USART2_RX_BUFFERSIZE_50);
                get_sys_time();
                date_flag = 0;
                dt_flag = 0;
            }

            if (time_flag) {
                parse_time(usart2_rx_buffer);
                usart2_send("\r\n");
                get_sys_time();
                time_flag = 0;
                dt_flag = 0;
            }

            // Assignment 7, task 2.2
            // case: set Time and Date, LED will be turned off for a silence purpose
            if (usart2_rx_buffer[0] == 'd') {
                strcpy(usart2_tx_buffer, "Enter date in format DD/MM/YY!\r\n");
                date_flag = 1;
                dt_flag = 1;
            }

            if (usart2_rx_buffer[0] == 't') {
                strcpy(usart2_tx_buffer, "Enter time in format HH/MM/SS!\r\n");
                time_flag = 1;
                dt_flag = 1;
            }

            if (usart2_rx_buffer[0] == 'p') {
                get_sys_time();
            }

            usart2_send(usart2_tx_buffer);
            memset(usart2_rx_buffer,0x00,USART2_RX_BUFFERSIZE_50);
            j=0;
        }
        else
        {
            usart2_rx_buffer[j] = c;
            j++;
            if (j >= USART2_RX_BUFFERSIZE_50) { j = 0; }
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

            else {
                //strcpy(usart2_tx_buffer, "Nur 1, 2 oder 4 sind erwartet!\r\n");
                // Assignment 4, task 2.7
                sprintf(usart2_tx_buffer, "  Zeichenkette=%s Länge=%d\r\n", usart2_rx_buffer, j);
            }

            usart2_send(usart2_tx_buffer);
            memset(usart2_rx_buffer,0x00,USART2_RX_BUFFERSIZE_50);
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
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x02;
    /* Set sub priority */
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x02;
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
    sprintf(date_buf, "%02i", hours);
    usart2_send(date_buf);
    usart2_send(":");

    int minutes = bcd_decimal(time.RTC_Minutes);
    sprintf(date_buf, "%02i", minutes);
    usart2_send(date_buf);
    usart2_send(":");

    int seconds = bcd_decimal(time.RTC_Seconds);
    sprintf(date_buf, "%02i", seconds);
    usart2_send(date_buf);
    usart2_send("\r\n");
}

void usart2_send_date(RTC_DateTypeDef date) {
    usart2_send("Date: ");
    int day = bcd_decimal(date.RTC_Date);
    sprintf(date_buf, "%02i", day);
    usart2_send(date_buf);
    usart2_send(".");
    int month = bcd_decimal(date.RTC_Month);
    sprintf(date_buf, "%02i", month);
    usart2_send(date_buf);
    usart2_send(".");
    int year = bcd_decimal(date.RTC_Year);
    sprintf(date_buf, "%02i", year);
    usart2_send(date_buf);
    usart2_send("\r\n");

}

void get_sys_only_time() {
    RTC_TimeTypeDef sTime;

    // FORMAT is RTC_Format_BIN) || RTC_Format_BCD
    // With these functions we copy data from TRC registers to our two variables (sTime and sDate).
    RTC_GetTime(RTC_Format_BCD, &sTime);

    // The data is BCD coded so we need to
    // convert a binary-coded decimal number into a decimal number in terms of representation
    usart2_send_time(sTime);
}

void get_sys_time() {
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    // FORMAT is RTC_Format_BIN) || RTC_Format_BCD
    // With these functions we copy data from TRC registers to our two variables (sTime and sDate).
    RTC_GetTime(RTC_Format_BCD, &sTime);
    RTC_GetDate(RTC_Format_BCD, &sDate);

    // The data is BCD coded so we need to
    // convert a binary-coded decimal number into a decimal number in terms of representation
    usart2_send_time(sTime);
    usart2_send_date(sDate);
}


uint8_t byte2bcd(uint8_t Value)
{
    uint8_t bcdhigh = 0;

    while (Value >= 10)
    {
        bcdhigh++;
        Value -= 10;
    }

    return  ((uint8_t)(bcdhigh << 4) | Value);
}

void parse_date(char * rx_buf) {
    usart2_send("New date to set: ");
    usart2_send(rx_buf);
    usart2_send("\r\n");

    RTC_DateTypeDef sDate;
    uint32_t date;
    uint32_t month;
    uint32_t year;

    uint32_t r = sscanf(rx_buf, "%0i/%0i/%0i", &date, &month, &year);

    if (r < 3) {
        usart2_send("Invalid date format\r\n");
        return;
    }
    memset(&sDate, 0, sizeof(RTC_DateTypeDef));

    sDate.RTC_Date = byte2bcd(date);
    sDate.RTC_Month = byte2bcd(month);
    sDate.RTC_Year = byte2bcd(year);

    if (ERROR == RTC_SetDate(RTC_Format_BCD, &sDate))
    {
        usart2_printf("Error setting date");
    }
}

void parse_time(char * rx_buf) {
    usart2_send("New time to set: ");
    usart2_send(rx_buf);
    usart2_send("\r\n");

    RTC_TimeTypeDef sTime;
    uint32_t hour;
    uint32_t minutes;
    uint32_t seconds;

    uint32_t r = sscanf(rx_buf, "%0i/%0i/%0i", &hour, &minutes, &seconds);

    if (r < 3) {
        usart2_send("Invalid time format\r\n");
        return;
    }

    memset(&sTime, 0, sizeof(RTC_TimeTypeDef));
    sTime.RTC_Hours= byte2bcd(hour);
    sTime.RTC_Minutes = byte2bcd(minutes);
    sTime.RTC_Seconds = byte2bcd(seconds);

    // RTC_SetDate(RTC_Format_BCD, &sDate);
    RTC_SetTime(RTC_Format_BCD, &sTime);

}


RTC_TimeTypeDef RTC_Time_Current;
RTC_DateTypeDef RTC_Date_Current;
RTC_AlarmTypeDef RTC_Alarm;

RTC_AlarmTypeDef RTC_Alarm_Struct;

// this function is a modificated version of set_RCT_Alarm_in from rtc.c
void set_RTC_Alarm(uint8_t weekday, uint8_t Std, uint8_t Min, uint8_t Sek, uint32_t RTC_AlarmMask) {

    char alarmOutput[128];

    // disable alarm
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

    RTC_Alarm_Struct.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
    RTC_Alarm_Struct.RTC_AlarmTime.RTC_Hours = Std;
    RTC_Alarm_Struct.RTC_AlarmTime.RTC_Minutes = Min;
    RTC_Alarm_Struct.RTC_AlarmTime.RTC_Seconds = Sek;
    RTC_Alarm_Struct.RTC_AlarmDateWeekDay = weekday;
    RTC_Alarm_Struct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_WeekDay;
    // set alarm mask
    RTC_Alarm_Struct.RTC_AlarmMask = RTC_AlarmMask;

    sprintf(alarmOutput, "RTC alarm set to %d:%d:%d on the %d(rd/th) day of the month\r\n",
            RTC_Alarm_Struct.RTC_AlarmTime.RTC_Hours,
            RTC_Alarm_Struct.RTC_AlarmTime.RTC_Minutes,
            RTC_Alarm_Struct.RTC_AlarmTime.RTC_Seconds,
            RTC_Alarm_Struct.RTC_AlarmDateWeekDay);
    usart2_send_text(alarmOutput);

    // init RTC alarm A register
    RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_Alarm_Struct);
    // disable RTC alarm A interrupt
    RTC_ITConfig(RTC_IT_ALRA, DISABLE);
    // disable alarm
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
    // reset flag
    RTC_ClearFlag(RTC_FLAG_ALRAF);

    // enable alarm interrupt
    RTC_ITConfig(RTC_IT_ALRA, ENABLE);
    // enable alarm
    RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
}

void print_weekday_of_alarm() {
    RTC_GetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_Alarm);
    char data[50] = { 0 };
    if (IS_RTC_WEEKDAY(RTC_Alarm.RTC_AlarmDateWeekDay)) {
        usart2_send_text("Alarm used weekday\r\n");
    } else
    {
        usart2_send_text("Alarm used Date\r\n");
        return;
    }
    char *weekdays[] = {"Mo", "Tue", "Wen", "Thu", "Fr"};
    sprintf(data, "Alarm was set to the following weekday: %s.\r\n", weekdays[RTC_Alarm.RTC_AlarmDateWeekDay-1]);
    usart2_send_text(data);
}

void set_RTC_Alarm_Mondays() { // alarm each monday at 00:30
    set_RTC_Alarm(RTC_Weekday_Monday, 0, 30, 0, RTC_AlarmMask_None);
    alarm_type = RTC_MONDAY_ALARM;
    print_weekday_of_alarm();
}

void set_RTC_Alarm_Thirds() { // alarm each 30 secs of a minute
    alarm_type = RTC_THRIDS_ALARM;
    set_RTC_Alarm(RTC_Weekday_Saturday, 0, 0, 30, RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Hours | RTC_AlarmMask_Minutes); // mask anything but seconds
    show_RTC_Alarm();
}

// this function is also a modificated version of set_RCT_Alarm_in from rtc.c
void set_RTC_Alarm_each_25_secs() { // every 25 secs from the first time the function is calles
    alarm_type = RTC_EVERY_25_SECS_ALARM;
    _Bool setzen_moeglich = false;
    char alarmOutput[128];

    // disable alarm
    RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

    // get current time and date
    RTC_GetTime(RTC_Format_BIN, &RTC_Time_Current);
    RTC_GetDate(RTC_Format_BIN, &RTC_Date_Current);


    RTC_Alarm_Struct.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
    RTC_Alarm_Struct.RTC_AlarmTime.RTC_Hours = RTC_Time_Current.RTC_Hours;
    RTC_Alarm_Struct.RTC_AlarmTime.RTC_Minutes = RTC_Time_Current.RTC_Minutes;
    RTC_Alarm_Struct.RTC_AlarmTime.RTC_Seconds = RTC_Time_Current.RTC_Seconds + 25;
    RTC_Alarm_Struct.RTC_AlarmDateWeekDay = RTC_Date_Current.RTC_Date;
    RTC_Alarm_Struct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
    // set alarm mask
    RTC_Alarm_Struct.RTC_AlarmMask = RTC_AlarmMask_None;

    // handles any time/date overflow, meaning e.g. if RTC_Time_Current.RTC_Seconds + 25 > 59 etc.
    // function was already implemented in rtc.c
    setzen_moeglich = Zeit_ueberlauf_Korektur(&RTC_Alarm_Struct);

    // if time/data overflow could be handled, set alarm
    if (setzen_moeglich == true)
    {
        sprintf(alarmOutput, "RTC alarm set to %d:%d:%d on the %d(rd/th) day of the month\r\n",
                RTC_Alarm_Struct.RTC_AlarmTime.RTC_Hours,
                RTC_Alarm_Struct.RTC_AlarmTime.RTC_Minutes,
                RTC_Alarm_Struct.RTC_AlarmTime.RTC_Seconds,
                RTC_Alarm_Struct.RTC_AlarmDateWeekDay);
        usart2_send_text(alarmOutput);
        // init RTC alarm A register
        RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm_A, &RTC_Alarm_Struct);
        // disable RTC alarm A interrupt
        RTC_ITConfig(RTC_IT_ALRA, DISABLE);
        // disable alarm
        RTC_AlarmCmd(RTC_Alarm_A, DISABLE);
        // reset flag
        RTC_ClearFlag(RTC_FLAG_ALRAF);

        // enable alarm interrupt
        RTC_ITConfig(RTC_IT_ALRA, ENABLE);
        // enable alarm
        RTC_AlarmCmd(RTC_Alarm_A, ENABLE);
    }
}

void sleep_mode_test() {
    int i;

    while(1) {
        // at restart blink with interval
        if (led_flag) {
            LED_GR_TOGGLE;
            // wait 0.5 sec to allow human to push a button on every loop
            wait_uSek(500000);
        }

        for (i = 0; i <= 500000; i++) {
            // 2.5 sec in 500000 iterations to allow human push a button
            // we poll the button state every 5 microseconds,
            // the every click should be detected due the contact bounce
            wait_uSek(5);

            // poll the button
            if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == 0) {
                // turn the led
                GR_LED_ON;
                led_flag = 0;
                // prevent contact bounce
                wait_uSek(300000);
                // The Sleep mode is entered by using the __WFI() or __WFE() functions
                usart2_send("Led ON, Sleep Mode Start\r\n");
                /* Request Wait For Interrupt */
                __WFI();
                usart2_send("LED OFF, sleep finished\r\n");
                usart2_send("Continue from stop point\r\n");
            }
        }
    }
}

void stop_mode_test() {
    int i;

    while(1) {
        // at restart blink with interval
        if (led_flag) {
            LED_GR_TOGGLE;
            // wait 0.5 sec to allow human to push a button on every loop
            wait_uSek(500000);
        }

        for (i = 0; i <= 500000; i++) {
            // 2.5 sec in 500000 iterations to allow human push a button
            // we poll the button state every 5 microseconds,
            // the every click should be detected due the contact bounce
            wait_uSek(5);

            // poll the button
            if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == 0) {
                // turn the led
                GR_LED_ON;
                led_flag = 0;
                // prevent contact bounce
                usart2_send("Led ON, Stop Mode Start\r\n");
                wait_uSek(300000);
                // The Stop mode is entered using the PWR_EnterSTOPMode
                // The voltage regulator can be configured either in normal or low-power mode.
                // STOP mode in entered with WFI instruction
                PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
                wait_uSek(300000);

                usart2_send("LED OFF, stop finished\r\n");
                usart2_send("Continue from stop point\r\n");
            }
        }
    }
}

EXTI_InitTypeDef EXTI_InitStruct;
NVIC_InitTypeDef NVIC_InitStruct;
void enable_RTC_Wakup(void) {

	EXTI_ClearITPendingBit(EXTI_Line22);
	EXTI_InitStruct.EXTI_Line = EXTI_Line22;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	RTC_WakeUpCmd(DISABLE);

    RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
    RTC_SetWakeUpCounter(61440);
    
    // Clear PWR Wakeup WUF Flag
	PWR_ClearFlag(PWR_CSR_WUF);
	PWR_WakeUpPinCmd(ENABLE);

    // Clear RTC Wakeup WUTF Flag
	RTC_ClearITPendingBit(RTC_IT_WUT);
	RTC_ClearFlag(RTC_FLAG_WUTF);

	RTC_ITConfig(RTC_IT_WUT, ENABLE);	// Bit 14
	RTC_AlarmCmd(RTC_CR_WUTE, ENABLE); 	// Bit 10

	RTC_WakeUpCmd(ENABLE);
}

void standby_mode_test() {
    usart2_send("STANDBY MODE TEST\r\n");
    RTC_WakeUpCmd(DISABLE);
    while(1) {
        wait_mSek(1000);
        LED_GR_TOGGLE;
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) == 0) {
            LED_GR_ON;
            usart2_send("Enable Wakeup\r\n");
            RTC_WakeUpCmd(ENABLE);

            usart2_send("StandBy Mode Start\r\n");
            SysTick->CTRL  &= ~SysTick_CTRL_TICKINT_Msk;
            wait_uSek_CC3100(2000000);

            PWR_EnterSTANDBYMode();

            while(1){;}
        }
    }
}

void start_stop_timer(TIM_TypeDef* TIMx, FunctionalState NewState) {
    TIM_SetCounter(TIMx, 0);
    TIM_ITConfig(TIMx, TIM_IT_Update, NewState);
    TIM_Cmd(TIMx, NewState);
}

void init_timer_7() {
    used_timer = USE_TIM7;
    
    // configure interrupt controller
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);
    
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    
    // configure timer 7
    TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1; // 100µs = 8400 * 1/84000000Hz 
    TIM_TimeBaseStructure.TIM_Period = 10 - 1;   // 1ms = 10 * 100µs
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;


    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);

    TIM_SetCounter(TIM7, 0);

    TIM_ClearITPendingBit(TIM7, TIM_IT_Update);
    
    /* Assignment 9 2.2 */
    // for this task the interrupt and timer must be enabled later on in the button 1 interrupt
    //TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE);
    //TIM_Cmd(TIM7, ENABLE);
 
}

void init_timer_6() {
    used_timer = USE_TIM6;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_TimeBaseStructure.TIM_Prescaler = 84 - 1; 
    TIM_TimeBaseStructure.TIM_Period = 1000 - 1; 
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure);
    TIM_SetCounter(TIM6, 0);

    //TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    
    // enable interrupt
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    
    // enable timer
    TIM_Cmd(TIM6, ENABLE);
 
}


void init_timer_5() {
    used_timer = USE_TIM5;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_StructInit(&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;

    GPIO_Init(GPIOA, &GPIO_InitStructure);
    

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource1, GPIO_AF_TIM5);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    TIM_TimeBaseStructure.TIM_Prescaler = 8400 - 1;
    TIM_TimeBaseStructure.TIM_Period = 10000 -1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

    TIM5->CCMR1 |= TIM_CCMR1_CC2S_0;

    TIM5->CCMR1 |= TIM_CCMR1_IC2F_2 + TIM_CCMR1_IC2F_1 + TIM_CCMR1_IC2F_0;

    TIM5->CCER   |= TIM_CCER_CC2P;

    TIM5->CCMR1   &= ~(TIM_CCMR1_IC2PSC_1 + TIM_CCMR1_IC2PSC_0);

    TIM5->CCER   |= TIM_CCER_CC2E;

    TIM5->DIER   |= TIM_DIER_CC2IE;
    
    
    // configure interrupt controller
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_SetCounter(TIM5, 0);

    // clear interrupt flag
    TIM_ClearITPendingBit(TIM5, TIM_IT_Update);
    
    // enable interrupt
    TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);

    // enable timer
    TIM_Cmd(TIM5, ENABLE);
}

void tim3_monitor_button_1_usage() {

    TIM_Cmd(TIM3, DISABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
 
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_StructInit(&GPIO_InitStructure);

    // configure port line 8
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;

    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // enable alternative function
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource8, GPIO_AF_TIM3);

    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    
    TIM3 -> CCMR1 |= TIM_CCMR1_CC1S_0 ;
    TIM3 -> CR2 |= TIM_CR2_TI1S ;
    
    // polarity
    TIM3 -> CCER |= TIM_CCER_CC1P ;
     
    TIM3 -> SMCR |= TIM_SMCR_SMS + TIM_SMCR_TS_2 + TIM_SMCR_ETF_0;
    

    // configure interrupt controller
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
 
    // configure TIM3
    TIM_TimeBaseStructure.TIM_Prescaler = 1;
    TIM_TimeBaseStructure.TIM_Period = 10 - 1; // interrupt gets triggered after 10 button 1 usages
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_SetCounter (TIM3, 0);

    // clear flag
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    // enable interrupt
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
    
    // enable timer 3
    TIM_Cmd(TIM3, ENABLE);
}

uint32_t rand_num;
int rand_latency;
char rand_buffer[60];
float durations[10];
void reflex_test(int round) {
    if (round < 10) {
        rand_num = Zufallszahl();
        rand_latency = 2000000+((int)(((float)rand_num)/100))%8000001; // between 2 and 10 sec in usec
        wait_uSek(rand_latency);
        LED_GR_ON;
        reflex_round_active = 1;
        start_stop_timer(TIM7, ENABLE);
    } else {
        reflex_test_runs = 0;
        usart2_send("Reflex test is done :)\r\n");
        int j;
        float min = durations[0];
        float max = durations[0];
        float average = 0;
        for (j=0; j<10; j++) {
            average += durations[j];
            if (durations[j] < min) {
                min = durations[j];
            }
            if (max < durations[j]) {
                max = durations[j];
            }
        }
        average /= 10;
        sprintf(rand_buffer, "Min: %f seconds\r\nMax: %f seconds\r\nAverage: %f seconds\r\n", min, max, average);
		usart2_send(rand_buffer);
    }
}

void handle_reflex_input() {
    start_stop_timer(TIM7, DISABLE);
    LED_GR_OFF;
    reflex_round_active = 0;
    durations[reflex_test_round] = (float)timer_interrupt_count/1000; // save duration in seconds (instead of msec)
    timer_interrupt_count = 0;
    reflex_test_round++;
    reflex_test(reflex_test_round);
}

void DMA1_Stream5_IRQHandler(void) {

    if (DMA_GetITStatus(DMA1_Stream5 , DMA_IT_TCIF5)) {

        DMA_ClearITPendingBit(DMA1_Stream5 , DMA_IT_TCIF5 );
    }
}
void DMA1_Stream6_IRQHandler(void) {

    if (DMA_GetITStatus(DMA1_Stream6 , DMA_IT_TCIF6)) {

        usart2_send("\r\nDMA IRQ fired\r\n");

        DMA_ClearITPendingBit(DMA1_Stream6 , DMA_IT_TCIF6);
    }
}


void init_DMA1_Stream5() {
    // Configure priority group
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2 );

    // Create NVIC Register Struct
    NVIC_InitTypeDef  NVIC_InitStructure;

    // Configure DMA2 Stream6 interrupt
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    // Now the DMA is configured
    // Switch on clock system for the DMA
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 , ENABLE );

    // Reset DMA stream register
    DMA_DeInit(DMA1_Stream5);

    // Create struct for the DMA register
    DMA_InitTypeDef   DMA_InitStructure;

    // Set DMA register in the struct
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)dma_usart2_rx;
    DMA_InitStructure.DMA_BufferSize = (uint16_t)strlen(dma_usart2_rx);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t )&USART2->DR;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    // Write values set in the struct to the DMA register
    DMA_Init(DMA1_Stream5 , &DMA_InitStructure );

    // Enable DMA interface for the USART in transmit mode
    USART_DMACmd(USART2 , USART_DMAReq_Rx , ENABLE );

    // Enable DMA stream interrupt for Transfer Complete
    DMA_ITConfig(DMA1_Stream5 , DMA_IT_TC , ENABLE );

    // Enable DMA transmit mode stream
    DMA_Cmd(DMA1_Stream5 , ENABLE );


}

void init_DMA1_Stream6() {
    // Configure priority group
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

    // Create NVIC Register Struct
    NVIC_InitTypeDef  NVIC_InitStructure;

    // Configure DMA2 Stream6 interrupt
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);

    // Now the DMA is configured
    // Switch on clock system for the DMA
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 , ENABLE );

    // Reset DMA stream register
    DMA_DeInit(DMA1_Stream6);

    // Create struct for the DMA register
    DMA_InitTypeDef   DMA_InitStructure;

    // Set DMA register in the struct
    DMA_InitStructure.DMA_Channel = DMA_Channel_5;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)usart2_tx_buffer;
    DMA_InitStructure.DMA_BufferSize = (uint16_t)strlen(usart2_tx_buffer);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t )&USART2->DR;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

    // Write values set in the struct to the DMA register
    DMA_Init(DMA1_Stream6 , &DMA_InitStructure );

    // Enable DMA interface for the USART in transmit mode
    USART_DMACmd(USART2 , USART_DMAReq_Tx , ENABLE );

    // Enable DMA stream interrupt for Transfer Complete
    DMA_ITConfig(DMA1_Stream6 , DMA_IT_TC , ENABLE );

    // Enable DMA transmit mode stream
    DMA_Cmd(DMA1_Stream6 , ENABLE );

    // Now the data is transferred by the DMA81
    // when the transfer is finished, the ISR is called4
}

void init_USART2_TX_DMA() {
    // Prepare initialization structures
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // Activate clock system
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    // GPIOA Configuration: USART2 TX on PA2 RX on PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Define alternative function
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // USART initialize
    USART_InitStructure.USART_BaudRate = 921600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);

    // Deinitialize the DMA
    DMA_DeInit(DMA1_Stream6); //USART2_TX_DMA_STREAM

    // Structure auf default setzen
    DMA_StructInit(&DMA_InitStructure);

    // Deaktiviere den DMA Transfer
    DMA_Cmd(DMA1_Stream6, DISABLE);

    //// Lösche das DMA-Flag
    DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);

    // Set DMA register in the struct
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)USART2_TX_BUF;
    DMA_InitStructure.DMA_BufferSize = USART2_BUFFERSIZE;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t )&USART2->DR;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream6, &DMA_InitStructure);

    // Fordere RX und TX an
    //USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);

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

    // Configure DMA2 Stream6 interrupt
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

void usart2_send_DMA(char *buffer) {
    if (buffer) {
        int length = strlen(buffer);
        if (length <= USART2_TX_BUFFERSIZE)
        {
            // Wait for the last transfer to complete
            while (my_usart2_running && DMA_GetFlagStatus(DMA1_Stream6, DMA_FLAG_TCIF6) == RESET) asm("");
            DMA_ClearFlag(DMA1_Stream6, DMA_FLAG_TCIF6);

            // Set uart flag
            my_usart2_running = 1;

            // Copy the string into the TX buffer
            strcpy(usart2_tx_buffer, buffer);

            // Enter the package length (nested so that only one calculation is necessary)
            DMA_SetCurrDataCounter(DMA1_Stream6, (unsigned short)length);

            // Activate the DMA transfer
            DMA_Cmd(DMA1_Stream6, ENABLE);
        }
    }
}

void USART2_IRQHandler_DMA() {
    char c;
    static int j = 0;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        c = (char)USART_ReceiveData(USART2);
        if (c=='\r')	// End of string input
        {
            usart2_rx_buffer[j] = 0x00 ;
            // copy to buffer for DMA transfer
            strcpy(USART2_TX_BUF, usart2_rx_buffer);

            // clear
            memset(usart2_rx_buffer, 0x00, USART2_BUFFERSIZE);
            j=0;
        }
        else
        {
            usart2_rx_buffer[j] = c;
            j++;
            if (j >= USART2_BUFFERSIZE) { j = 0; }
        }
    }
}


void init_USART2_TX_DMA_swp() {
    // Prepare initialization structures
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;

    // Activate clock system
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    // GPIOA Configuration: USART2 TX on PA2 RX on PA3
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // Define alternative function
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // USART initialize
    USART_InitStructure.USART_BaudRate = 921600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);

    // Deinitialize the DMA
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

    // Configure DMA2 Stream6 interrupt
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init (&NVIC_InitStructure);
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}
