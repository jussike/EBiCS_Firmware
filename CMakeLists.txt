cmake_minimum_required(VERSION 3.20)

project(LishuiFOC_01 C ASM)

set(CMAKE_C_STANDARD 11)

set(CPU_FLAGS
    -mcpu=cortex-m3
    -mthumb
    -mfloat-abi=soft
)

set(SOURCES
    Src/main.c
    Src/FOC.c
    Src/print.c
    Src/stm32f1xx_it.c
    Src/stm32f1xx_hal_msp.c
    Src/system_stm32f1xx.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c
    Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c
    startup/startup_stm32f103x6.s
)

add_executable(${PROJECT_NAME}.elf ${SOURCES})

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    ${CPU_FLAGS}
    -g2
    -Og
    -Wall
    -ffunction-sections
    -fdata-sections
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    STM32F103x6
    USE_HAL_DRIVER
    ARM_MATH_CM3
    "__weak=__attribute__((weak))"
    "__packed=__attribute__((__packed__))"
)

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    Inc
    Drivers/STM32F1xx_HAL_Driver/Inc
    Drivers/STM32F1xx_HAL_Driver/Inc/Legacy
    Drivers/CMSIS/Device/ST/STM32F1xx/Include
    Drivers/CMSIS/Include
)

set(LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/STM32F103C6Tx_FLASH.ld)
target_link_options(${PROJECT_NAME}.elf PRIVATE
    ${CPU_FLAGS}
    -specs=nosys.specs
    -specs=nano.specs
    -T${LINKER_SCRIPT}
    -Wl,-Map=${PROJECT_NAME}.map
    -Wl,--gc-sections
)

target_link_libraries(${PROJECT_NAME}.elf PRIVATE m arm_cortexM3l_math)
target_link_directories(${PROJECT_NAME}.elf PRIVATE Drivers/CMSIS)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND arm-none-eabi-objcopy -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    COMMAND arm-none-eabi-objcopy -O binary ${PROJECT_NAME}.elf ${PROJECT_NAME}.bin
    COMMAND arm-none-eabi-size ${PROJECT_NAME}.elf
)
