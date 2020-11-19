target_sources(STM32F415RG.elf PRIVATE 
    BME280.c 
    bme280_driver.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)
