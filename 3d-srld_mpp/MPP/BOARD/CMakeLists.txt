target_sources(STM32F415RG.elf PRIVATE 
    beeper.c 
    client_ftp.c 
    client_ntp.c 
    i2c.c 
    init.c 
    interrupts.c 
    led.c 
    power.c 
    rtc.c 
    taster.c 
    usart.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)
