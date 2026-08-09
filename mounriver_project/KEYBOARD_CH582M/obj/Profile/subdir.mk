################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Profile/battservice.c \
../Profile/devinfoservice.c \
../Profile/hiddev.c \
../Profile/hidkbmservice.c \
../Profile/scanparamservice.c 

C_DEPS += \
./Profile/battservice.d \
./Profile/devinfoservice.d \
./Profile/hiddev.d \
./Profile/hidkbmservice.d \
./Profile/scanparamservice.d 

OBJS += \
./Profile/battservice.o \
./Profile/devinfoservice.o \
./Profile/hiddev.o \
./Profile/hidkbmservice.o \
./Profile/scanparamservice.o 


EXPANDS += \
./Profile/battservice.c.253r.expand \
./Profile/devinfoservice.c.253r.expand \
./Profile/hiddev.c.253r.expand \
./Profile/hidkbmservice.c.253r.expand \
./Profile/scanparamservice.c.253r.expand 



# Each subdirectory must supply rules for building sources it contributes
Profile/%.o: ../Profile/%.c
	@	riscv-wch-elf-gcc -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -march=rv32imac_zicsr -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g -DCLK_OSC32K=1 -DLOW_MEM=1 -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/FatFs" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/FatFs/port" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/StdPeriphDriver/inc" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/class/hid" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/class/msc" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/common" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/core" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Startup" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/HAL/include" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/APP/include" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Profile/include" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Ld" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/LIB" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

