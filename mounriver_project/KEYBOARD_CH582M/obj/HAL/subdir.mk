################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/BATTERY.c \
../HAL/BLE.c \
../HAL/CORE.c \
../HAL/FATFS.c \
../HAL/HW_I2C.c \
../HAL/I2C_TP.c \
../HAL/KEYBOARD.c \
../HAL/MCU.c \
../HAL/MPR121.c \
../HAL/OLED.c \
../HAL/OLEDFONT.c \
../HAL/OLED_UI.c \
../HAL/OLED_UI_CFG.c \
../HAL/RF_PHY.c \
../HAL/RTC.c \
../HAL/SLEEP.c \
../HAL/USB.c \
../HAL/WS2812.c 

C_DEPS += \
./HAL/BATTERY.d \
./HAL/BLE.d \
./HAL/CORE.d \
./HAL/FATFS.d \
./HAL/HW_I2C.d \
./HAL/I2C_TP.d \
./HAL/KEYBOARD.d \
./HAL/MCU.d \
./HAL/MPR121.d \
./HAL/OLED.d \
./HAL/OLEDFONT.d \
./HAL/OLED_UI.d \
./HAL/OLED_UI_CFG.d \
./HAL/RF_PHY.d \
./HAL/RTC.d \
./HAL/SLEEP.d \
./HAL/USB.d \
./HAL/WS2812.d 

OBJS += \
./HAL/BATTERY.o \
./HAL/BLE.o \
./HAL/CORE.o \
./HAL/FATFS.o \
./HAL/HW_I2C.o \
./HAL/I2C_TP.o \
./HAL/KEYBOARD.o \
./HAL/MCU.o \
./HAL/MPR121.o \
./HAL/OLED.o \
./HAL/OLEDFONT.o \
./HAL/OLED_UI.o \
./HAL/OLED_UI_CFG.o \
./HAL/RF_PHY.o \
./HAL/RTC.o \
./HAL/SLEEP.o \
./HAL/USB.o \
./HAL/WS2812.o 


EXPANDS += \
./HAL/BATTERY.c.253r.expand \
./HAL/BLE.c.253r.expand \
./HAL/CORE.c.253r.expand \
./HAL/FATFS.c.253r.expand \
./HAL/HW_I2C.c.253r.expand \
./HAL/I2C_TP.c.253r.expand \
./HAL/KEYBOARD.c.253r.expand \
./HAL/MCU.c.253r.expand \
./HAL/MPR121.c.253r.expand \
./HAL/OLED.c.253r.expand \
./HAL/OLEDFONT.c.253r.expand \
./HAL/OLED_UI.c.253r.expand \
./HAL/OLED_UI_CFG.c.253r.expand \
./HAL/RF_PHY.c.253r.expand \
./HAL/RTC.c.253r.expand \
./HAL/SLEEP.c.253r.expand \
./HAL/USB.c.253r.expand \
./HAL/WS2812.c.253r.expand 



# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@	riscv-wch-elf-gcc -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -march=rv32imac_zicsr -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g -DCLK_OSC32K=1 -DLOW_MEM=1 -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/FatFs" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/FatFs/port" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/StdPeriphDriver/inc" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/class/hid" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/class/msc" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/common" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/core" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/Startup" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/HAL/include" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/APP/include" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/Profile/include" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/Ld" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/LIB" -I"e:/Project/tp78_v2/mounriver_project/KEYBOARD_CH582M/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

