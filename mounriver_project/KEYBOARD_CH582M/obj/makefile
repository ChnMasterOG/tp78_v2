################################################################################
# MRS Version: 1.9.0
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include StdPeriphDriver/subdir.mk
-include Startup/subdir.mk
-include RVMSIS/subdir.mk
-include Profile/subdir.mk
-include Packages/FatFs/port/subdir.mk
-include Packages/FatFs/subdir.mk
-include Packages/CherryUSB/core/subdir.mk
-include Packages/CherryUSB/class/msc/subdir.mk
-include Packages/CherryUSB/class/hid/subdir.mk
-include Packages/CherryUSB/subdir.mk
-include HAL/subdir.mk
-include APP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
BLE_CH582M.hex \

SECONDARY_LIST += \
BLE_CH582M.lst \

SECONDARY_SIZE += \
BLE_CH582M.siz \


# All Target
all: BLE_CH582M.elf secondary-outputs

# Tool invocations
BLE_CH582M.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g -T "E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"../" -L"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\LIB" -L"E:\Project\tp78_v2\mounriver_project\KEYBOARD_CH582M\StdPeriphDriver" -Xlinker --print-memory-usage -Wl,-Map,"BLE_CH582M.map" --specs=nano.specs --specs=nosys.specs -o "BLE_CH582M.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
BLE_CH582M.hex: BLE_CH582M.elf
	@	riscv-none-embed-objcopy -O ihex "BLE_CH582M.elf"  "BLE_CH582M.hex"
	@	@
BLE_CH582M.lst: BLE_CH582M.elf
	@	riscv-none-embed-objdump --demangle --debugging --line-numbers -D "BLE_CH582M.elf" > "BLE_CH582M.lst"
	@	@
BLE_CH582M.siz: BLE_CH582M.elf
	@	riscv-none-embed-size --format=berkeley "BLE_CH582M.elf"
	@	@
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) BLE_CH582M.elf
	-@
post-build:
	-riscv-none-embed-objcopy -O binary "BLE_CH582M.elf" "BLE_CH582M.bin"
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents post-build

-include ../makefile.targets
