target_sources(STM32F415RG.elf PRIVATE 
    MPU9250.c 
    mpu9250_driver.c 
    mpu9250_math.c 
    mpu9250_serial.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)
