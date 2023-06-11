################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../StdPeriphDriver/CH58x_adc.c \
../StdPeriphDriver/CH58x_clk.c \
../StdPeriphDriver/CH58x_flash.c \
../StdPeriphDriver/CH58x_gpio.c \
../StdPeriphDriver/CH58x_i2c.c \
../StdPeriphDriver/CH58x_pwm.c \
../StdPeriphDriver/CH58x_pwr.c \
../StdPeriphDriver/CH58x_sys.c \
../StdPeriphDriver/CH58x_timer0.c \
../StdPeriphDriver/CH58x_timer1.c \
../StdPeriphDriver/CH58x_timer2.c \
../StdPeriphDriver/CH58x_uart0.c \
../StdPeriphDriver/CH58x_uart1.c \
../StdPeriphDriver/CH58x_uart2.c \
../StdPeriphDriver/CH58x_uart3.c \
../StdPeriphDriver/CH58x_usbdev.c 

OBJS += \
./StdPeriphDriver/CH58x_adc.o \
./StdPeriphDriver/CH58x_clk.o \
./StdPeriphDriver/CH58x_flash.o \
./StdPeriphDriver/CH58x_gpio.o \
./StdPeriphDriver/CH58x_i2c.o \
./StdPeriphDriver/CH58x_pwm.o \
./StdPeriphDriver/CH58x_pwr.o \
./StdPeriphDriver/CH58x_sys.o \
./StdPeriphDriver/CH58x_timer0.o \
./StdPeriphDriver/CH58x_timer1.o \
./StdPeriphDriver/CH58x_timer2.o \
./StdPeriphDriver/CH58x_uart0.o \
./StdPeriphDriver/CH58x_uart1.o \
./StdPeriphDriver/CH58x_uart2.o \
./StdPeriphDriver/CH58x_uart3.o \
./StdPeriphDriver/CH58x_usbdev.o 

C_DEPS += \
./StdPeriphDriver/CH58x_adc.d \
./StdPeriphDriver/CH58x_clk.d \
./StdPeriphDriver/CH58x_flash.d \
./StdPeriphDriver/CH58x_gpio.d \
./StdPeriphDriver/CH58x_i2c.d \
./StdPeriphDriver/CH58x_pwm.d \
./StdPeriphDriver/CH58x_pwr.d \
./StdPeriphDriver/CH58x_sys.d \
./StdPeriphDriver/CH58x_timer0.d \
./StdPeriphDriver/CH58x_timer1.d \
./StdPeriphDriver/CH58x_timer2.d \
./StdPeriphDriver/CH58x_uart0.d \
./StdPeriphDriver/CH58x_uart1.d \
./StdPeriphDriver/CH58x_uart2.d \
./StdPeriphDriver/CH58x_uart3.d \
./StdPeriphDriver/CH58x_usbdev.d 


# Each subdirectory must supply rules for building sources it contributes
StdPeriphDriver/%.o: ../StdPeriphDriver/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -DCLK_OSC32K=2 -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\FatFs" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\FatFs\port" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\StdPeriphDriver\inc" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\CherryUSB" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\CherryUSB\class\hid" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\CherryUSB\class\msc" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\CherryUSB\common" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Packages\CherryUSB\core" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Startup" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\HAL\include" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\APP\include" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Profile\include" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\Ld" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\LIB" -I"E:\Project\tp78_v2\mounriver_project\KBD_CH582M\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

