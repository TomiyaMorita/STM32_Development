################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include TMC_API/tmc/ramp/subdir.mk
-include TMC_API/tmc/ic/TMC2209/subdir.mk
-include TMC_API/tmc/helpers/subdir.mk
-include TMC_API/tmc/subdir.mk
-include TMC_API/hal/Startrampe/tmc/subdir.mk
-include TMC_API/hal/Startrampe/st/lib/src/subdir.mk
-include TMC_API/hal/Startrampe/st/subdir.mk
-include TMC_API/hal/Landungsbruecke/tmc/subdir.mk
-include TMC_API/hal/Landungsbruecke/freescale/USB_CDC/subdir.mk
-include TMC_API/hal/Landungsbruecke/freescale/subdir.mk
-include TMC_API/boards/subdir.mk
-include TMC_API/subdir.mk
-include Drivers/STM32F3xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
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


BUILD_ARTIFACT_NAME := STM32F303_TMC_SteppigTest
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
STM32F303_TMC_SteppigTest.elf \

MAP_FILES += \
STM32F303_TMC_SteppigTest.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STM32F303_TMC_SteppigTest.list \


# All Target
all: main-build

# Main-build Target
main-build: STM32F303_TMC_SteppigTest.elf secondary-outputs

# Tool invocations
STM32F303_TMC_SteppigTest.elf STM32F303_TMC_SteppigTest.map: $(OBJS) $(USER_OBJS) /Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/STM32F303K8TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "STM32F303_TMC_SteppigTest.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/STM32F303K8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="STM32F303_TMC_SteppigTest.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STM32F303_TMC_SteppigTest.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "STM32F303_TMC_SteppigTest.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) STM32F303_TMC_SteppigTest.elf STM32F303_TMC_SteppigTest.list STM32F303_TMC_SteppigTest.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
