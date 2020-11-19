add_subdirectory(kernel)
add_subdirectory(portable)
target_include_directories(STM32F415RG.elf SYSTEM PRIVATE .)
