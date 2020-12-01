#include "init.h"



// Zuweisung Handle USE_DEVICE_MODE
__ALIGN_BEGIN USB_OTG_CORE_HANDLE USB_OTG_dev __ALIGN_END;


//=========================================================================
void InitSysTick(void)
//=========================================================================
{
	// Bei f= 168000000Hz und Teiler 100 gibt es alle 10ms einen SysTick
	// SysTick Intervall = (1/f) * (SystemCoreClock / 100) = 0,01 Sekunde
	// SysTick Intervall = (1/f) * (SystemCoreClock / 1000) = 0,001 Sekunde
	if (SysTick_Config(SystemCoreClock / 1000)) { while (1); }	// 1ms
	}



//=========================================================================
void init_board(void)
//=========================================================================
{
	//timer_init();
	//timer_start();
	//=====================================================================
	//==========LTC2950 - Power-Control
	//=====================================================================
	init_POWER_ON();


	//=====================================================================
	//==========UART - Serielle Schnittstelle; 8N1, 921600 Bit/s
	//=====================================================================
	usart2_init();
	usart2_send("\r\n\r\n=====Systemstart=====\r\n");


	//=====================================================================
	//==========USB - Virtuelle Serielle Schnittstelle
	//=====================================================================
//		USBD_Init(&USB_OTG_dev, 	// pdev: device instance
//				USB_OTG_FS_CORE_ID, // core_address: USB OTG core ID
//				&USR_desc, 			// Device Descriptor
//				&USBD_CDC_cb, 		// Device Call Back
//				&USR_cb 			// usr_cb: User callback structure address
//				);
	usart2_send("=> USB\r\n");


	//=====================================================================
	//==========SD-Card mit FAT-File System
	//=====================================================================
	sd_card_init();
	usart2_send("=> SD-CARD\r\n");


	//=====================================================================
	//==========CC3100 - WLAN Transceiver Init und Firmware Check
	//=====================================================================
	usart2_send("=> CC3100\r\n");
	CC3100_init();
	CC3100_fw_check();


	//=====================================================================
	//==========I2C Interface für BNO055 BME280 TAC6416
	//=====================================================================
	init_I2C2();
	usart2_send("=> BME280\r\n");


	//=====================================================================
	//==========MPU-9250
	//=====================================================================
	mpu9250_init();		// MPU aus
	mpu9250_uninit();	// 28,000ms


	//=====================================================================
	//==========DW1000 UWB Transceiver
	//=====================================================================
	DW1000_init();		// 3,600mSek
	uwb_deepsleep();	// muss dann wieder geweckt werden ToDo !!!
	usart2_send("=> DW1000\r\n");


	//=====================================================================
	//==========LED
	//=====================================================================
	//init_LED();
	//usart2_send("=> LED\r\n");

	//=====================================================================
	// 	Tasten Taster1 () Taster2 ()
	//=====================================================================
	//init_TASTER1();
	//init_TASTER2();
	//init_Taster1_IRQ8();
	//init_Taster2_IRQ5();
	//usart2_send("=> TASTER\r\n");

	//=====================================================================
	//==========Signalgeber
	//=====================================================================
	init_BEEPER();
	usart2_send("=> BEEPER\r\n");

	//=====================================================================
	//==========RTC Batterie - Spannungsmessung
	//=====================================================================
	read_VBAT_RTC();

	usart2_send("=====================\r\n");

	beep(4000,200,0);

	//timer_stop("Initialisierungszeit");
}

//=========================================================================
void StartBootLoader(unsigned int BootLoaderStatus)
//=========================================================================
{
//	SysMemBootJump=(void (*)(void)) (*((u32*) 0x1fff0004));
//	if (BootLoaderStatus==1)
//		{
//		RCC_DeInit();
//		SysTick->CTRL=0;
//		SysTick->LOAD=0;
//		SysTick->VAL=0;
//		//RCC_SYSCLKConfig(RCC_SYSCLKSource_HSI);
//		__set_PRIMASK(1);		// Disable Interrupts
//		__set_MSP(0x200010000); // Set the main stack pointer
//		SysMemBootJump();
//		while(1){;}
//		}
	}






