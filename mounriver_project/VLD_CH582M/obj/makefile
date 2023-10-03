################################################################################
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
VLD_CH582M.hex \

SECONDARY_LIST += \
VLD_CH582M.lst \

SECONDARY_SIZE += \
VLD_CH582M.siz \


# All Target
all: VLD_CH582M.elf secondary-outputs

# Tool invocations
VLD_CH582M.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common  -g -T "../Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -L"E:\tp78_v2\mounriver_project\VLD_CH582M\StdPeriphDriver" -L"E:\tp78_v2\mounriver_project\VLD_CH582M\LIB" -Xlinker --print-memory-usage -Wl,-Map,"VLD_CH582M.map" --specs=nano.specs --specs=nosys.specs -o "VLD_CH582M.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
VLD_CH582M.hex: VLD_CH582M.elf
	@	riscv-none-embed-objcopy -O ihex "VLD_CH582M.elf"  "VLD_CH582M.hex"
	@	@
VLD_CH582M.lst: VLD_CH582M.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "VLD_CH582M.elf" > "VLD_CH582M.lst"
	@	@
VLD_CH582M.siz: VLD_CH582M.elf
	@	riscv-none-embed-size --format=berkeley "VLD_CH582M.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) VLD_CH582M.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
