################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include tmc/ramp/subdir.mk
-include tmc/ic/TMC2209/subdir.mk
-include tmc/helpers/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
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


BUILD_ARTIFACT_NAME := STMF446_ArmRemoteRobot_Test
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
STMF446_ArmRemoteRobot_Test.elf \

MAP_FILES += \
STMF446_ArmRemoteRobot_Test.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STMF446_ArmRemoteRobot_Test.list \


# All Target
all: main-build

# Main-build Target
main-build: STMF446_ArmRemoteRobot_Test.elf secondary-outputs

# Tool invocations
STMF446_ArmRemoteRobot_Test.elf STMF446_ArmRemoteRobot_Test.map: $(OBJS) $(USER_OBJS) /Users/moritatomiya/Documents/GitHub/COPREC_RemoteControl_Software/STM/STMF446_ArmRemoteRobot_Test/STM32F446RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "STMF446_ArmRemoteRobot_Test.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/Users/moritatomiya/Documents/GitHub/COPREC_RemoteControl_Software/STM/STMF446_ArmRemoteRobot_Test/STM32F446RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="STMF446_ArmRemoteRobot_Test.map" -Wl,--gc-sections -static -u_printf_float --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STMF446_ArmRemoteRobot_Test.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "STMF446_ArmRemoteRobot_Test.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) STMF446_ArmRemoteRobot_Test.elf STMF446_ArmRemoteRobot_Test.list STMF446_ArmRemoteRobot_Test.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
