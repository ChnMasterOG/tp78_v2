################################################################################
# MRS Version: 1.9.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RVMSIS/core_riscv.c 

OBJS += \
./RVMSIS/core_riscv.o 

C_DEPS += \
./RVMSIS/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
RVMSIS/%.o: ../RVMSIS/%.c
	@	@	riscv-none-elf-gcc -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -march=rv32imac_zicsr -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DCLK_OSC32K=1 -DLOW_MEM=1 -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\FatFs" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\FatFs\port" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\StdPeriphDriver\inc" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\CherryUSB" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\CherryUSB\class\hid" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\CherryUSB\class\msc" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\CherryUSB\common" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Packages\CherryUSB\core" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Startup" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\HAL\include" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\APP\include" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Profile\include" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Ld" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\LIB" -I"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

