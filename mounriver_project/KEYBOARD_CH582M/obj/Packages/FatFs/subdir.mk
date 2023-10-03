################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Packages/FatFs/diskio.c \
../Packages/FatFs/ff.c \
../Packages/FatFs/fftools.c \
../Packages/FatFs/ffunicode.c 

OBJS += \
./Packages/FatFs/diskio.o \
./Packages/FatFs/ff.o \
./Packages/FatFs/fftools.o \
./Packages/FatFs/ffunicode.o 

C_DEPS += \
./Packages/FatFs/diskio.d \
./Packages/FatFs/ff.d \
./Packages/FatFs/fftools.d \
./Packages/FatFs/ffunicode.d 


# Each subdirectory must supply rules for building sources it contributes
Packages/FatFs/%.o: ../Packages/FatFs/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DCLK_OSC32K=2 -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\FatFs" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\FatFs\port" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\StdPeriphDriver\inc" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\CherryUSB" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\CherryUSB\class\hid" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\CherryUSB\class\msc" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\CherryUSB\common" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\CherryUSB\core" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\Startup" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\HAL\include" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\APP\include" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\Profile\include" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\Ld" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\LIB" -I"E:\tp78_v2\mounriver_project\KEYBOARD_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

