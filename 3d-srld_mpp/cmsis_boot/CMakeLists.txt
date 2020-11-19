target_sources(STM32F415RG.elf PRIVATE 
    startup/startup_stm32f4xx.c 
    system_stm32f4xx.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)
