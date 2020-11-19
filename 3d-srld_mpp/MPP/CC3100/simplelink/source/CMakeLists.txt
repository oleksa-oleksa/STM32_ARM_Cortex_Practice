target_sources(STM32F415RG.elf PRIVATE 
    device.c 
    driver.c 
    flowcont.c 
    fs.c 
    netapp.c 
    netcfg.c 
    nonos.c 
    socket.c 
    spawn.c 
    wlan.c)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE ../include/)
