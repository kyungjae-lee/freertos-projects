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


BUILD_ARTIFACT_NAME := 03_LED_Tasks_Delay
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
03_LED_Tasks_Delay.elf \

MAP_FILES += \
03_LED_Tasks_Delay.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
03_LED_Tasks_Delay.list \


# All Target
all: main-build

# Main-build Target
main-build: 03_LED_Tasks_Delay.elf secondary-outputs

# Tool invocations
03_LED_Tasks_Delay.elf 03_LED_Tasks_Delay.map: $(OBJS) $(USER_OBJS) /home/klee/repos/freertos-projects/Workspace/03_LED_Tasks_Delay/STM32F407VGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "03_LED_Tasks_Delay.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/home/klee/repos/freertos-projects/Workspace/03_LED_Tasks_Delay/STM32F407VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="03_LED_Tasks_Delay.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

03_LED_Tasks_Delay.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "03_LED_Tasks_Delay.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) 03_LED_Tasks_Delay.elf 03_LED_Tasks_Delay.list 03_LED_Tasks_Delay.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
