target_sources(STM32F415RG.elf PRIVATE 
    CC3100_Board.c 
    CC3100.c 
    CC3100_EventHandler.c 
    CC3100_HttpServer.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)

add_subdirectory(simplelink/source)
