#include "interrupts.h"
#include "usart.h"

void hard_fault_handler_c(unsigned int * hardfault_args);

//=========================================================================
void NMI_Handler(void)
{
	while (1){ ; }
}

//=========================================================================
void HardFault_Handler(void)
{
	asm ("TST LR, #4");
	asm ("ITE EQ");
	asm ("MRSEQ R0, MSP");
	asm ("MRSNE R0, PSP");
	asm ("B hard_fault_handler_c");
}

//=========================================================================
void MemManage_Handler(void)
{
	while (1){ ; }
}

//=========================================================================
void BusFault_Handler(void)
{
	while (1){ ; }
}

//=========================================================================
void UsageFault_Handler(void)
{
	while (1){ ; }
}

//=========================================================================
void SVC_Handler(void)
{
	while (1){ ; }
}

//=========================================================================
void DebugMon_Handler(void)
{
	while (1){ ; }
}

//=========================================================================
//void PendSV_Handler(void){
//}

//=========================================================================
void SysTick_Handler(void)
{
	static unsigned long stc_led = 0;
	static unsigned long stc0 = 0;
	static unsigned long stc1 = 0;
	static unsigned long stc2 = 0;
	stc_led++;
	stc0++;
	stc1++;
	stc2++;

    //======================================================================
    // Assignment 3: control LED with Clock System
    // TASK 3: Time delay with "timer" variable
    // blocking wait with while-loop

    timer--;

/*  //======================================================================
    // Assignment 3: control LED with Clock System
    // TASK 2: for 0.5 sec ist LED ON and for 3 sec is ON
    static  unsigned  long  SysTickCounter = 0;
    SysTickCounter ++;
    // 3000 ms turned on + 500 ms was turned off = 3500 ms
    if (SysTickCounter  == 3500) {
        LED_GR_ON;
        SysTickCounter = 0;
    }
    else if (SysTickCounter  == 500) {
        LED_GR_OFF;
    }*/

	//======================================================================
    // We will control LED toggle in SysTick also
    // because otherwise with while(1)-loop will be blocked
    // the UART-2 IRQ Handler
    static  unsigned  long  SysTickCounter = 0;
    SysTickCounter++;
    if (SysTickCounter  == led_timer) {
        GR_LED_TOGGLE;
        //usart2_send("LED toggled\r\n");
    }
    // overflow
    // if timer is set to the lower number
    // we have to catch the new lower number outside the toogle
    if (SysTickCounter  >= led_timer) {
        SysTickCounter = 0;
    }
    //======================================================================


	// DW1000 Timeout
	systickcounter += 1;
	if ( stc0 >= 20 )
		{
			//uwbranging_tick();
			stc0 = 0;
		}



	//======================================================================
	//	CoOS_SysTick_Handler alle 10ms in CoOs arch.c aufrufen
	// nur Einkommentieren wenn CoOS genutzt wird
	if ( stc1 >= 10 )
		{
	//		CoOS_SysTick_Handler();
			stc1 = 0;
		}

	//======================================================================
	// CC3100 alle 50ms Sockets aktualisieren
	if (stc2 >= 5)
		{
			stc2 = 0;
			if ( (IS_CONNECTED(WiFi_Status)) && (IS_IP_ACQUIRED(WiFi_Status)) && (!Stop_CC3100_select) && (!mqtt_run) )
			{
			CC3100_select(); // nur aktiv wenn mit AP verbunden
			}
			else
			{
			_SlNonOsMainLoopTask();
			}
		}
	//======================================================================
	// SD-Card
	sd_card_SysTick_Handler();

	//======================================================================
	// MQTT
	MQTT_SysTickHandler();

	//======================================================================
	// LED zyklisch schalten
	/*
	if ( stc_led >= 500 )
		{
			LED_GR_TOGGLE;
			stc_led = 0;
		}
	 */
}


//=========================================================================
void WWDG_IRQHandler(void)
{

	unsigned char value_watchdog_counter = 0x7F;
    WWDG_ClearFlag();
    WWDG_DeInit();
	WWDG_SetCounter(value_watchdog_counter);
    WWDG_ClearFlag();
    usart2_send("WWDG_IRQn\r\n");
}


//=========================================================================
void EXTI0_IRQHandler(void)
{
	//SystemInit();
	//===== CC3100
	if(EXTI_GetITStatus(EXTI_Line0) == SET)
		{
			EXTI_ClearFlag(EXTI_Line0);
			EXTI_ClearITPendingBit(EXTI_Line0);
			WLAN_intHandler();
		}
}


//=========================================================================
void EXTI1_IRQHandler(void)
{
	//SystemInit();

	// Setze den Interrupt zurück
	EXTI->PR = EXTI_Line1;

	// IRQ Aktiv
	dw1000_irqactive = 1;

	// Handhabe den DW1000 interrupt
	dw1000_handleInterrupt();

	// IRQ nicht mehr aktiv
	dw1000_irqactive = 0;
}


//=========================================================================
void EXTI2_IRQHandler(void)
{
	//===== nicht belegt
	if(EXTI_GetITStatus(EXTI_Line2) == SET)
		{
			EXTI_ClearFlag(EXTI_Line2);
			EXTI_ClearITPendingBit(EXTI_Line2);
			// nicht belegt
		    usart2_send("EXTI2_IRQn\r\n");
		}
}


//=========================================================================
void EXTI3_IRQHandler(void)
{
	//===== nicht belegt
	if(EXTI_GetITStatus(EXTI_Line3) == SET)
		{
			EXTI_ClearFlag(EXTI_Line3);
			EXTI_ClearITPendingBit(EXTI_Line3);
			// nicht belegt
		    usart2_send("EXTI3_IRQn\r\n");
		}
}


//=========================================================================
void EXTI4_IRQHandler(void)
{	//===== MPU-9250
	if(EXTI_GetITStatus(EXTI_Line4) == SET)
		{
			EXTI_ClearFlag(EXTI_Line4);
			EXTI_ClearITPendingBit(EXTI_Line4);
			// ISR Aufruf
		    usart2_send("EXTI4_IRQn\r\n");
		}
}


//=========================================================================
void EXTI9_5_IRQHandler(void)
{
	//SystemInit();
	//===== Taster2
	if (EXTI_GetITStatus(EXTI_Line5) == SET)
		{
			EXTI_ClearFlag(EXTI_Line5);
			EXTI_ClearITPendingBit(EXTI_Line5);
			usart2_send("EXTI5_IRQn\r\n");
			TASTER2_IRQ();

		}
	//===== nicht belegt
	if (EXTI_GetITStatus(EXTI_Line6) == SET)
		{
			EXTI_ClearFlag(EXTI_Line6);
			EXTI_ClearITPendingBit(EXTI_Line6);
		    usart2_send("EXTI6_IRQn\r\n");
		}
	//===== nicht belegt
	if (EXTI_GetITStatus(EXTI_Line7) == SET)
		{
			EXTI_ClearFlag(EXTI_Line7);
			EXTI_ClearITPendingBit(EXTI_Line7);
		    usart2_send("EXTI7_IRQn\r\n");
		}
	//===== Taster 1
	if (EXTI_GetITStatus(EXTI_Line8) == SET)
		{
			EXTI_ClearFlag(EXTI_Line8);
			EXTI_ClearITPendingBit(EXTI_Line8);
		    usart2_send("EXTI8_IRQn\r\n");
			TASTER1_IRQ();
		}
	//===== nicht belegt
	if (EXTI_GetITStatus(EXTI_Line9) == SET)
		{
			EXTI_ClearFlag(EXTI_Line9);
			EXTI_ClearITPendingBit(EXTI_Line9);
		    usart2_send("EXTI9_IRQn\r\n");
		}

}


//=========================================================================
void EXTI15_10_IRQHandler(void)
{
    //========================
    // Assignment 6: Interrupts
    /* Make sure that interrupt flag is set */
    /* PC8 is connected to EXTI_Line8 */
    if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
        /* PC8 CASE: */
        // The ISR should switch on the green LED on PB2.
        GR_LED_ON;

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line12);
    }


    //========================
    // some staff the we got from repo, let it be here
	//SystemInit();
	//===== nicht belegt
	if(EXTI_GetITStatus(EXTI_Line10) == SET)
		{
			EXTI_ClearFlag(EXTI_Line10);
			EXTI_ClearITPendingBit(EXTI_Line10);
		    usart2_send("EXTI10_IRQn\r\n");
		}
	//===== nicht belegt
	if(EXTI_GetITStatus(EXTI_Line11) == SET)
		{
			EXTI_ClearFlag(EXTI_Line11);
			EXTI_ClearITPendingBit(EXTI_Line11);
		    usart2_send("EXTI11_IRQn\r\n");
		}
	//===== nicht belegt
	if (EXTI_GetITStatus(EXTI_Line12) == SET)
		{
			EXTI_ClearFlag(EXTI_Line12);
			EXTI_ClearITPendingBit(EXTI_Line12);
		    usart2_send("EXTI12_IRQn\r\n");
		}
	//===== nicht belegt
	if(EXTI_GetITStatus(EXTI_Line13) == SET)
		{
			EXTI_ClearFlag(EXTI_Line13);
			EXTI_ClearITPendingBit(EXTI_Line13);
		    usart2_send("EXTI13_IRQn\r\n");
		}
	//===== nicht belegt
	if(EXTI_GetITStatus(EXTI_Line14) == SET)
		{
			EXTI_ClearFlag(EXTI_Line14);
			EXTI_ClearITPendingBit(EXTI_Line14);
		    usart2_send("EXTI14_IRQn\r\n");
		}
	//===== nicht belegt
	if(EXTI_GetITStatus(EXTI_Line15) == SET)
		{
			EXTI_ClearFlag(EXTI_Line15);
			EXTI_ClearITPendingBit(EXTI_Line15);
		    usart2_send("EXTI15_IRQn\r\n");
		}

}


//=========================================================================
void RTC_Alarm_IRQHandler(void)
{
	//SystemInit();
	//===== Time Stamp interrupt
	if(RTC_GetITStatus(RTC_IT_TS) != RESET)
		{
			usart2_send("\r\nTime Stamp ALARM\r\n");
			RTC_ClearITPendingBit(RTC_IT_TS);
			EXTI_ClearITPendingBit(EXTI_Line21);
		}
	//=====	WakeUp Timer interrupt
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
		{
			usart2_send("\r\nWakeUp _ALARM\r\n");
			RTC_ClearITPendingBit(RTC_IT_WUT);
			EXTI_ClearITPendingBit(EXTI_Line22);
		}
	//===== RTC_IT_ALRB: Alarm B interrupt
	if(RTC_GetITStatus(RTC_IT_ALRB) != RESET)
		{
			usart2_send("\r\nALARM ALRB\r\n");
			RTC_ClearITPendingBit(RTC_IT_ALRB);
			EXTI_ClearITPendingBit(EXTI_Line17);
		}
	//===== RTC_IT_ALRA: Alarm A interrupt
	if(RTC_GetITStatus(RTC_IT_ALRA) != RESET)
		{
			usart2_send("\r\nALARM ALRA\r\n");
			LED_GR_TOGGLE;
			RTC_ClearITPendingBit(RTC_IT_ALRA);
			EXTI_ClearITPendingBit(EXTI_Line17);
			//	if (RTC_Alarm_CallBack[0] != NULL)
			//	{
			//	RTC_Alarm_CallBack[0]();
			//	wait_uSek(3000000);
			//	}
		}
	//===== RTC_IT_TAMP1: Tamper 1 event interrupt
	if(RTC_GetITStatus(RTC_IT_TAMP1) != RESET)
		{
			usart2_send("\r\nTamper 1 ALARM\r\n");
			RTC_ClearITPendingBit(RTC_IT_TAMP1);
			EXTI_ClearITPendingBit(EXTI_Line21);
		}

}


//=========================================================================
void ADC_IRQHandler(void){
	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
		{
			// ... Code für Ende Wandlung
			ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
		}
	if(ADC_GetITStatus(ADC1, ADC_IT_AWD) == SET)
		{
			// ... Code für analogen Watchdog
			ADC_ClearITPendingBit(ADC1, ADC_IT_AWD);
		}
    	usart2_send("ADC_IRQn\r\n");
}

//Interrupt handler declaration
// In my development environment,
// this handler is declared in my startup assembly file as a weak reference to Default_Handle
// so as long as I provide a new declaration and implementation of this interrupt handler,
// the weak reference will be replaced
//=========================================================================
void USART2_IRQHandler(void)
{
	//===== USART2
    //usart2_send("USART2_IRQn\r\n");
    // original function
    //USART2_IRQ();
    USART2_IRQ_LED_CONTROL();
}


//=========================================================================
void UART5_IRQHandler(void)
{
	//===== USART5
	if (USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
		{
			USART_SendData(USART2, (char)USART_ReceiveData(UART5));
		}
    //usart2_send("USART5_IRQn\r\n");
}


//=========================================================================
void USART6_IRQHandler(void)
{
	//===== USART6
	//USART6_IRQ();
	CC3100_uart6_receive_IRQ();
    //usart2_send("USART6_IRQn\r\n");
}

//=========================================================================
void DMA2_Stream6_IRQHandler(void)
{
	//===== DMA2_Stream6
	//DMA2_Stream6_IRQ();
    //usart2_send("DMA2_Stream6_IRQn\r\n");
}

//=========================================================================
void RTC_WKUP_IRQHandler(void)
{
	//SystemInit();
	if(RTC_GetITStatus(RTC_IT_WUT) != RESET)
		{
	    	usart2_send("RTC_WKUP_IRQn\r\n");
			RTC_ClearITPendingBit(RTC_IT_WUT);
			EXTI_ClearITPendingBit(EXTI_Line22);
		}
}


//=========================================================================
void TIM5_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM5, TIM_IT_CC2) == SET)
		{
			TIM_ClearITPendingBit(TIM1, TIM_IT_CC2);
		}
    //usart2_send("TIM5_IRQn\r\n");
}


//=========================================================================
void TIM7_IRQHandler(void)
{
	BEEPER_IRQHandler();
    //usart2_send("TIM7_IRQn\r\n");
}

//=========================================================================
void DMA2_Stream2_IRQHandler(void)
{
	//RXD stream der SPI1
	if (DMA_GetITStatus(DMA2_Stream2, DMA_IT_TCIF2))
	{
	DMA_ClearITPendingBit(DMA2_Stream2, DMA_IT_TCIF2);
	}
    //usart2_send("DMA2_Stream2_IRQn\r\n");
}

//=========================================================================
void DMA2_Stream3_IRQHandler(void)
{
	//TXD stream der SPI1
	if (DMA_GetITStatus(DMA2_Stream3, DMA_IT_TCIF3))
	{
	DMA_ClearITPendingBit(DMA2_Stream3, DMA_IT_TCIF3);
	}
    //usart2_send("DMA2_Stream3_IRQn\r\n");
}




//=========================================================================
void TIM6_DAC_IRQHandler()
{
	if((TIM6->SR) & (TIM_SR_UIF != 0) )
		{
			//LED_GR_TOGGLE;
		}
	TIM6->SR &= ~TIM_SR_UIF;
    //usart2_send("TIM6_IRQn\r\n");
}


//=========================================================================
// From Joseph Yiu, minor edits by FVH
// hard fault handler in C,
// with stack frame location as input parameter
// called from HardFault_Handler in file
void hard_fault_handler_c(unsigned int * hardfault_args)
{
	char out[256];
	unsigned int stacked_r0;
	unsigned int stacked_r1;
	unsigned int stacked_r2;
	unsigned int stacked_r3;
	unsigned int stacked_r12;
	unsigned int stacked_lr;
	unsigned int stacked_pc;
	unsigned int stacked_psr;

	stacked_r0 = ((unsigned long) hardfault_args[0]);
	stacked_r1 = ((unsigned long) hardfault_args[1]);
	stacked_r2 = ((unsigned long) hardfault_args[2]);
	stacked_r3 = ((unsigned long) hardfault_args[3]);

	stacked_r12 = ((unsigned long) hardfault_args[4]);
	stacked_lr = ((unsigned long) hardfault_args[5]);
	stacked_pc = ((unsigned long) hardfault_args[6]);
	stacked_psr = ((unsigned long) hardfault_args[7]);

	uart_send("\r\n[Hard fault handler - all numbers in hex]\r\n");
	sprintf(out, "R0  = %x\r\n", stacked_r0);
	uart_send(out);
	sprintf(out, "R1  = %x\r\n", stacked_r1);
	uart_send(out);
	sprintf(out, "R2  = %x\r\n", stacked_r2);
	uart_send(out);
	sprintf(out, "R3  = %x\r\n", stacked_r3);
	uart_send(out);
	sprintf(out, "R12 = %x\r\n", stacked_r12);
	uart_send(out);
	sprintf(out, "LR [R14] = %x  subroutine call return address\r\n", stacked_lr);
	uart_send(out);
	sprintf(out, "PC [R15] = %x  program counter\r\n", stacked_pc);
	uart_send(out);
	sprintf(out, "PSR  = %x\r\n", stacked_psr);
	uart_send(out);
	sprintf(out, "BFAR = %lx\r\n", (*((volatile unsigned long *) (0xE000ED38))));
	uart_send(out);
	sprintf(out, "CFSR = %lx\r\n", (*((volatile unsigned long *) (0xE000ED28))));
	uart_send(out);
	sprintf(out, "HFSR = %lx\r\n", (*((volatile unsigned long *) (0xE000ED2C))));
	uart_send(out);
	sprintf(out, "DFSR = %lx\r\n", (*((volatile unsigned long *) (0xE000ED30))));
	uart_send(out);
	sprintf(out, "AFSR = %lx\r\n", (*((volatile unsigned long *) (0xE000ED3C))));
	uart_send(out);
	sprintf(out, "SCB_SHCSR = %x\r\n", (unsigned int) SCB->SHCSR);
	uart_send(out);

	if (SCB->HFSR & SCB_HFSR_DEBUGEVT_Msk) {
		uart_send("##This is a DEBUG FAULT##\r\n");

	} else if (SCB->HFSR & SCB_HFSR_FORCED_Msk) {
		uart_send("##This is a FORCED FAULT##\r\n");

//		if (SCB->CFSR & (0x1 << SCB_CFSR_USGFAULTSR_Msk)) {
//			uart_send("undefined instruction\r\n");
//
//		} else if (SCB->CFSR & (0x2 << SCB_CFSR_USGFAULTSR_Pos)) {
//			uart_send("instruction makes illegal use of the EPSR\r\n");
//
//		} else if (SCB->CFSR & (0x4 << SCB_CFSR_USGFAULTSR_Pos)) {
//			uart_send("Invalid PC load UsageFault, caused by an invalid PC load by EXC_RETURN\r\n");
//
//		} else if (SCB->CFSR & (0x8 << SCB_CFSR_USGFAULTSR_Pos)) {
//			uart_send("The processor does not support coprocessor instructions\r\n");
//
//		} else if (SCB->CFSR & (0x100 << SCB_CFSR_USGFAULTSR_Pos)) {
//			uart_send("Unaligned access\r\n");
//
//		} else if (SCB->CFSR & (0x200 << SCB_CFSR_USGFAULTSR_Pos)) {
//			uart_send("Divide by zero\r\n");
//
//		}
	} else if (SCB->HFSR & SCB_HFSR_VECTTBL_Pos) {
		sprintf(out,"##This is a BUS FAULT##\r\n");
		uart_send(out);
	}
	uart_send("HARDFAULT HANDLER !!!!\r\n");
	while (1) {;}
}

