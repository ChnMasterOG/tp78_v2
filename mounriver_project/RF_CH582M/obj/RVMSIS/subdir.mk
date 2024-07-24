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
	@	@	riscv-none-elf-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -I"E:\Project\tp78_v2\mounriver_project\RF_CH582M\Startup" -I"E:\Project\tp78_v2\mounriver_project\RF_CH582M\Ld" -I"E:\Project\tp78_v2\mounriver_project\RF_CH582M\RVMSIS" -I"E:\Project\tp78_v2\mounriver_project\RF_CH582M\StdPeriphDriver\inc" -I"E:\Project\tp78_v2\mounriver_project\RF_CH582M\APP\include" -I"E:\Project\tp78_v2\mounriver_project\RF_CH582M\HAL\include" -I"E:\Project\tp78_v2\mounriver_project\RF_CH582M\LIB" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

