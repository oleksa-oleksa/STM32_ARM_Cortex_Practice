cmake_minimum_required(VERSION 3.13.0)
project(3D-SRLD_MPP C)

# Should probably be set in a toolchain.cmake file...
if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
    set(HOST_EXECUTABLE_SUFFIX "*.exe")
else ()
    set(HOST_EXECUTABLE_SUFFIX "")
endif ()

set(CMAKE_C_COMPILER "${CMAKE_SOURCE_DIR}/../gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-gcc${HOST_EXECUTABLE_SUFFIX}")
set(CMAKE_OBJCOPY "${CMAKE_SOURCE_DIR}/../gcc-arm-none-eabi-4_8-2014q3/bin/arm-none-eabi-objcopy${HOST_EXECUTABLE_SUFFIX}")
set(CMAKE_C_COMPILER_TARGET arm-none-eabi)
set(CMAKE_C_FLAGS "-mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb")

# The main sources
set(srcs main.c 
    aufgabe.c 
    test.c)
add_executable(STM32F415RG.elf ${srcs})
add_subdirectory(MPP)
add_subdirectory(cmsis_boot)
add_subdirectory(cmsis_lib/source)
add_subdirectory(CoOS)

# Preprocessor macros
target_compile_definitions(STM32F415RG.elf PRIVATE
    STM32F415RG
    STM32F4XX
    USE_STDPERIPH_DRIVER
    __ASSEMBLY__
    __FPU_USED
    )

# Compile options
target_compile_options(STM32F415RG.elf PRIVATE -ffunction-sections -Wall -O0 -g)

# Link flags:
# * linker script
# * linker map
# * garbage collection
# * debug info
# * don't add startup files
target_link_options(STM32F415RG.elf PRIVATE
    -Wl,-T${CMAKE_SOURCE_DIR}/arm-gcc-link.ld
    -Wl,-Map=STM32F415RG.map
    -Wl,--gc-sections
    -g
    -nostartfiles)
# Make link stage depend on linker script
set_target_properties(STM32F415RG.elf PROPERTIES
    LINK_DEPENDS "${CMAKE_SOURCE_DIR}/arm-gcc-link.ld")

# Add local dir to include dirs
target_include_directories(STM32F415RG.elf PRIVATE ./)

# Link with STM provided binary and libm
target_link_libraries(STM32F415RG.elf ${CMAKE_SOURCE_DIR}/libSTM32F415RG.a m)

# Build .bin file from .elf file (by default)
add_custom_target(STM32F415RG.bin ALL DEPENDS STM32F415RG.elf)
add_custom_command(
    TARGET STM32F415RG.bin
    COMMAND ${CMAKE_OBJCOPY} -O binary ${CMAKE_CURRENT_BINARY_DIR}/STM32F415RG.elf ${CMAKE_CURRENT_BINARY_DIR}/STM32F415RG.bin
    )

# Add flash target
add_custom_target(flash DEPENDS STM32F415RG.bin)
add_custom_command(TARGET flash
    USES_TERMINAL
    COMMAND st-flash write ${CMAKE_CURRENT_BINARY_DIR}/STM32F415RG.bin 0x8000000)

# Build documentation
find_package(Doxygen)
if (DOXYGEN_FOUND)
    configure_file(${CMAKE_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating documentation"
        VERBATIM
    )
else (DOXYGEN_FOUND)
    message("Doxygen needs to be installed to generate the documentation")
endif (DOXYGEN_FOUND)
