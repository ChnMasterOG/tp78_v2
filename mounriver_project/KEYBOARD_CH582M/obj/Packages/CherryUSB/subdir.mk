################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Packages/CherryUSB/usb_ch58x_dc_usbfs.c 

C_DEPS += \
./Packages/CherryUSB/usb_ch58x_dc_usbfs.d 

OBJS += \
./Packages/CherryUSB/usb_ch58x_dc_usbfs.o 


EXPANDS += \
./Packages/CherryUSB/usb_ch58x_dc_usbfs.c.253r.expand 



# Each subdirectory must supply rules for building sources it contributes
Packages/CherryUSB/%.o: ../Packages/CherryUSB/%.c
	@	riscv-wch-elf-gcc -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -march=rv32imac_zicsr -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g -DCLK_OSC32K=1 -DLOW_MEM=1 -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/FatFs" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/FatFs/port" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/StdPeriphDriver/inc" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/class/hid" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/class/msc" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/common" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Packages/CherryUSB/core" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Startup" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/HAL/include" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/APP/include" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Profile/include" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/Ld" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/LIB" -I"f:/keyboard/tp78_v2/mounriver_project/KEYBOARD_CH582M/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

