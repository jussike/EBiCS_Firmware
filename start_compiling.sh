#!/bin/sh

mkdir -p Debug
cd Debug
make -f ../make/makefile clean

cd ../make
cp * ../Debug
cd ../Debug
mkdir -p Drivers/STM32F1xx_HAL_Driver/Src
mkdir -p Startup
mkdir -p Src

cp subdir_drivers.mk Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
cp subdir_src.mk Src/subdir.mk
cp subdir_startup.mk Startup/subdir.mk

#cp STLINK_FLASH.ld ../STM32F103C6Tx_FLASH.ld
#cp STLINK_system_stm32f1xx.c ../Src/system_stm32f1xx.c

make all

#ST-LINK_CLI.exe -c SWD -P LishuiFOC_01.hex -V
