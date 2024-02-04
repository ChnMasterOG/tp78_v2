################################################################################
# MRS Version: 1.9.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Profile/battservice.c \
../Profile/devinfoservice.c \
../Profile/hiddev.c \
../Profile/hidmouseservice.c \
../Profile/scanparamservice.c 

OBJS += \
./Profile/battservice.o \
./Profile/devinfoservice.o \
./Profile/hiddev.o \
./Profile/hidmouseservice.o \
./Profile/scanparamservice.o 

C_DEPS += \
./Profile/battservice.d \
./Profile/devinfoservice.d \
./Profile/hiddev.d \
./Profile/hidmouseservice.d \
./Profile/scanparamservice.d 


# Each subdirectory must supply rules for building sources it contributes
Profile/%.o: ../Profile/%.c
	@	@	riscv-none-elf-gcc -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -march=rv32imac_zicsr -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -DCLK_OSC32K=1 -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\Startup" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\APP\include" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\Profile\include" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\StdPeriphDriver\inc" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\HAL\include" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\Ld" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\LIB" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

