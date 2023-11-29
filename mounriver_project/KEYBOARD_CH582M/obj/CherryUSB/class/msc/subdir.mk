################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CherryUSB/class/msc/usbd_msc.c 

OBJS += \
./CherryUSB/class/msc/usbd_msc.o 

C_DEPS += \
./CherryUSB/class/msc/usbd_msc.d 


# Each subdirectory must supply rules for building sources it contributes
CherryUSB/class/msc/%.o: ../CherryUSB/class/msc/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DDEBUG=3 -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\StdPeriphDriver\inc" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\CherryUSB" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\CherryUSB\class\hid" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\CherryUSB\class\msc" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\CherryUSB\common" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\CherryUSB\core" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Startup" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\HAL\include" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\APP\include" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Profile\include" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\Ld" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\LIB" -I"E:\Project\tp78_v2\mounriver_project\BLE_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

