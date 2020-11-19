target_sources(STM32F415RG.elf PRIVATE 
    MQTT.c 
    MQTTClient.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)
add_subdirectory(src)
