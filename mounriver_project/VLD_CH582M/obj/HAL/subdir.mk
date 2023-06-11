################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/KEY.c \
../HAL/LED.c \
../HAL/MCU.c \
../HAL/RTC.c \
../HAL/SLEEP.c 

OBJS += \
./HAL/KEY.o \
./HAL/LED.o \
./HAL/MCU.o \
./HAL/RTC.o \
./HAL/SLEEP.o 

C_DEPS += \
./HAL/KEY.d \
./HAL/LED.d \
./HAL/MCU.d \
./HAL/RTC.d \
./HAL/SLEEP.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -DCLK_OSC32K=1 -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\Startup" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\APP\include" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\Profile\include" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\StdPeriphDriver\inc" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\HAL\include" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\Ld" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\LIB" -I"E:\Project\tp78_v2\mounriver_project\VLD_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

