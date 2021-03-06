#ifndef __aufgabe_h__
#define __aufgabe_h__

//#########################################################################
//########## cmsis_lib include
//#########################################################################
//#include "stm32f4xx.h"
#include "misc.h"
//#include "stm32f4xx_adc.h"
//#include "stm32f4xx_can.h"
//#include "stm32f4xx_crc.h"
//#include "stm32f4xx_cryp_aes.h"
//#include "stm32f4xx_cryp_des.h"
//#include "stm32f4xx_cryp_tdes.h"
//#include "stm32f4xx_cryp.h"
//#include "stm32f4xx_dac.h"
//#include "stm32f4xx_dbgmcu.h"
//#include "stm32f4xx_dcmi.h"
//#include "stm32f4xx_dma.h"
#include "stm32f4xx_exti.h"
//#include "stm32f4xx_flash.h"
//#include "stm32f4xx_fsmc.h"
#include <stm32f4xx_rtc.h>
#include "stm32f4xx_gpio.h"
//#include "stm32f4xx_hash_md5.h"
//#include "stm32f4xx_hash_sha1.h"
//#include "stm32f4xx_hash.h"
//#include "stm32f4xx_i2c.h"
//#include "stm32f4xx_iwdg.h"
#include "stm32f4xx_pwr.h"
#include "stm32f4xx_rcc.h"
//#include "stm32f4xx_rng.h"
#include "stm32f4xx_rtc.h"
//#include "stm32f4xx_sdio.h"
//#include "stm32f4xx_spi.h"
//#include "stm32f4xx_syscfg.h"
//#include "stm32f4xx_tim.h"
//#include "stm32f4xx_usart.h"
//#include "stm32f4xx_wwdg.h"


//#########################################################################
//########## mpp_lib include
//#########################################################################
//#include "BME280.h"
//=========================================================================
#include "beeper.h"
//#include "client_ftp.h"
//#include "client_ntp.h"
//#include "global.h"
//#include "i2c.h"
//#include "init.h"
//#include "interrupts.h"
//#include "led.h"
//#include "power.h"
#include "rtc.h"
//#include "taster.h"
//#include "usart.h"
//=========================================================================
//#include "simplelink.h"
//#include "netapp.h"
//#include "CC3100.h"
#include "CC3100_Board.h"
//=========================================================================
//#include "dw1000_driver.h"
//#include "dw1000_ranging.h"
//#include "dw1000.h"
//=========================================================================
//#include "MQTT.h"
//=========================================================================
//#include "MPU9250.h"
//=========================================================================
//#include "SDCARD.h"
//=========================================================================
//#include "usbd_cdc_vcp.h"
//#include "usb_conf.h"
//=========================================================================
//#include "CoOS.h"
//#include "mpu9250_driver.h"


//#########################################################################
//########## standart_lib include
//#########################################################################
//#include "stdio.h"
//#include "string.h"
//#include "math.h"


//#########################################################################
//########## Eigene Funktionen, Macros und Variablen
//#########################################################################


//=========================================================================
// Macros
//=========================================================================
#define GR_LED_ON (GPIO_SetBits(GPIOB, GPIO_Pin_2))
#define GR_LED_TOGGLE (GPIO_ToggleBits(GPIOB, GPIO_Pin_2))
#define GR_LED_OFF (GPIO_ResetBits(GPIOB, GPIO_Pin_2))
#define USART2_TX_BUFFERSIZE_50 50
#define USART2_RX_BUFFERSIZE_50 1024
#define USART2_BUFFERSIZE 1024

#define RTC_MONDAY_ALARM 1
#define RTC_THRIDS_ALARM 2
#define RTC_EVERY_25_SECS_ALARM 3

#define USE_TIM5 5
#define USE_TIM6 6
#define USE_TIM7 7



//=========================================================================
// Variablen
//=========================================================================
//extern int counter;
//extern char counter_char;
extern int led_timer;
extern int alarm_type;
extern int led_flag;
extern int timer_interrupt_count;
extern int used_timer;
extern int timer_runs;
extern int reflex_test_round;
extern int reflex_test_runs;
extern int reflex_round_active;

extern char USART2_TX_BUF[USART2_BUFFERSIZE];
extern char USART2_RX_BUF[USART2_BUFFERSIZE];



//=========================================================================
// Funktionen
//=========================================================================
void our_init_board();

void init_leds(uint32_t RCC_AHB1Periph, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

void init_button_1();
void init_button_2();

void led_on_off();

void init_usart_2_tx();
void init_usart_2_tx_rx();
void usart2_send_text(char *chars);
void usart2_print();

void USART2_IRQ_LED_CONTROL();
void USART2_IRQ_LED_CONTROL_WITH_OFF();
void USART2_IRQHandler(void);
void USART2_IRQ_SET_DATATIME();

void init_button_1_irq();
void init_button_2_irq();

void button_1_handler();
void button_2_handler();

void deinit_button_1_irq();

void get_sys_only_time();
void get_sys_time();
void usart2_send_time(RTC_TimeTypeDef time);
int bcd_decimal(uint8_t hex);
void usart2_send_date(RTC_DateTypeDef date);


void USART2_IRQ_LED_CONTROL_WITH_OFF();
void USART2_GET_DATATIME(void);

void init_iwdg();

void init_button_1_irq();
void init_button_2_irq();

void button_1_handler();
void button_2_handler();

void deinit_button_1_irq();

void get_sys_time();
void usart2_send_time(RTC_TimeTypeDef time);
int bcd_decimal(uint8_t hex);
void usart2_send_date(RTC_DateTypeDef date);

void parse_date(char * rx_buf);
void parse_time(char * rx_buf);
uint32_t dec2bcd_r(uint16_t dec);
void get_sys_only_time();
void set_RTC_Alarm_Mondays();
void set_RTC_Alarm_Thirds();
void set_RTC_Alarm_each_25_secs();
void sleep_mode_test();
void stop_mode_test();
void standby_mode_test();
void enable_RTC_Wakup();

// Assignment 9
void init_timer_7();
void init_timer_6();
void init_timer_5();
void start_stop_timer(TIM_TypeDef* TIMx, FunctionalState NewState);
void tim3_monitor_button_1_usage();
void reflex_test();
void handle_reflex_input();

// Assignment 10 DMA
void DMA1_Stream5_IRQHandler(void);
void DMA1_Stream6_IRQHandler(void);
void init_DMA1_Stream5();
void init_DMA1_Stream6();
void init_USART2_TX_DMA();
void usart2_send_DMA(char *buffer);
void USART2_IRQHandler_DMA();

void init_USART2_TX_RX(void);
void init_USART2_RX_IRQ(void);

void list_access_points();


//=========================================================================
#endif
