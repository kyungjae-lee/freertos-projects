################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include Common/ThirdParty/SEGGER/SEGGER/Syscalls/subdir.mk
-include Common/ThirdParty/SEGGER/SEGGER/subdir.mk
-include Common/ThirdParty/SEGGER/Rec/subdir.mk
-include Common/ThirdParty/SEGGER/OS/subdir.mk
-include Common/ThirdParty/SEGGER/Config/subdir.mk
-include Common/ThirdParty/FreeRTOS/portable/MemMang/subdir.mk
-include Common/ThirdParty/FreeRTOS/portable/GCC/ARM_CM4F/subdir.mk
-include Common/ThirdParty/FreeRTOS/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
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

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := 08_Idle_Hook
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
08_Idle_Hook.elf \

MAP_FILES += \
08_Idle_Hook.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
08_Idle_Hook.list \


# All Target
all: main-build

# Main-build Target
main-build: 08_Idle_Hook.elf secondary-outputs

# Tool invocations
08_Idle_Hook.elf 08_Idle_Hook.map: $(OBJS) $(USER_OBJS) /home/klee/repos/freertos-projects/Workspace/08_Idle_Hook/STM32F407VGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "08_Idle_Hook.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/home/klee/repos/freertos-projects/Workspace/08_Idle_Hook/STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="08_Idle_Hook.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

08_Idle_Hook.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "08_Idle_Hook.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) 08_Idle_Hook.elf 08_Idle_Hook.list 08_Idle_Hook.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
