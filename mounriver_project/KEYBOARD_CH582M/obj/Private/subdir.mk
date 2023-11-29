################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Private/MPR121.c \
../Private/TP78_MPR121_LIB.c 

O_SRCS += \
../Private/TP78_MPR121_LIB.o 

OBJS += \
./Private/MPR121.o \
./Private/TP78_MPR121_LIB.o 

C_DEPS += \
./Private/MPR121.d \
./Private/TP78_MPR121_LIB.d 


# Each subdirectory must supply rules for building sources it contributes
Private/%.o: ../Private/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DCLK_OSC32K=2 -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Packages\FatFs" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Packages\FatFs\port" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\StdPeriphDriver\inc" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Packages\CherryUSB" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Packages\CherryUSB\class\hid" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Packages\CherryUSB\class\msc" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Packages\CherryUSB\common" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Packages\CherryUSB\core" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Startup" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\HAL\include" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\APP\include" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Profile\include" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Ld" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\LIB" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

