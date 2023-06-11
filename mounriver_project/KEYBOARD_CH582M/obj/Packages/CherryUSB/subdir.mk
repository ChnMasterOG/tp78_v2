################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Packages/CherryUSB/usb_ch58x_dc_usbfs.c 

OBJS += \
./Packages/CherryUSB/usb_ch58x_dc_usbfs.o 

C_DEPS += \
./Packages/CherryUSB/usb_ch58x_dc_usbfs.d 


# Each subdirectory must supply rules for building sources it contributes
Packages/CherryUSB/%.o: ../Packages/CherryUSB/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DCLK_OSC32K=2 -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\FatFs" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\FatFs\port" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\StdPeriphDriver\inc" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\CherryUSB" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\CherryUSB\class\hid" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\CherryUSB\class\msc" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\CherryUSB\common" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\CherryUSB\core" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Startup" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\HAL\include" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\APP\include" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Profile\include" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Ld" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\LIB" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

