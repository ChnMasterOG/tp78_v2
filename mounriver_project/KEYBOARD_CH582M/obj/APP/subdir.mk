################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/debug_log.c \
../APP/main.c \
../APP/snake.c \
../APP/tp78_via.c 

C_DEPS += \
./APP/debug_log.d \
./APP/main.d \
./APP/snake.d \
./APP/tp78_via.d 

OBJS += \
./APP/debug_log.o \
./APP/main.o \
./APP/snake.o \
./APP/tp78_via.o 


EXPANDS += \
./APP/debug_log.c.253r.expand \
./APP/main.c.253r.expand \
./APP/snake.c.253r.expand \
./APP/tp78_via.c.253r.expand 



# Each subdirectory must supply rules for building sources it contributes
APP/%.o: ../APP/%.c
	@	riscv-wch-elf-gcc -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -march=rv32imac_zicsr -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g -DCLK_OSC32K=1 -DLOW_MEM=1 -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/FatFs" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/FatFs/port" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/StdPeriphDriver/inc" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/class/hid" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/class/msc" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/common" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/core" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Startup" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/HAL/include" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/APP/include" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Profile/include" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Ld" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/LIB" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

