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
	arm-none-eabi-gcc -mcpu=cortex-m4 -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/tmc2209_test/tmc/ic/TMC2209 -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/tmc2209_test/tmc/helpers -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMC_SteppingTest2/TMC_API/TMC-EvalSystem/hal/Landungsbruecke/freescale/USB_CDC" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMC_SteppingTest2/TMC_API/TMC-EvalSystem/hal/Landungsbruecke/freescale" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32f303k8tx.d ./Core/Startup/startup_stm32f303k8tx.o

.PHONY: clean-Core-2f-Startup

