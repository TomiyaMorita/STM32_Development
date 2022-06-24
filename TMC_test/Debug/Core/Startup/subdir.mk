################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f303k8tx.s 

OBJS += \
./Core/Startup/startup_stm32f303k8tx.o 

S_DEPS += \
./Core/Startup/startup_stm32f303k8tx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/tmc/ic/TMC2209 -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Drivers -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Drivers/STM32F3xx_HAL_Driver -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f303k8tx.d ./Core/Startup/startup_stm32f303k8tx.o

.PHONY: clean-Core-2f-Startup

