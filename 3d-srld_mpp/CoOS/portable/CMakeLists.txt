target_sources(STM32F415RG.elf PRIVATE 
    arch.c 
    GCC/port.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)
