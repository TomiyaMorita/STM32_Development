################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/ramp/LinearRamp.c \
../Core/ramp/LinearRamp1.c \
../Core/ramp/Ramp.c 

OBJS += \
./Core/ramp/LinearRamp.o \
./Core/ramp/LinearRamp1.o \
./Core/ramp/Ramp.o 

C_DEPS += \
./Core/ramp/LinearRamp.d \
./Core/ramp/LinearRamp1.d \
./Core/ramp/Ramp.d 


# Each subdirectory must supply rules for building sources it contributes
Core/ramp/%.o Core/ramp/%.su: ../Core/ramp/%.c Core/ramp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Device/ST/STM32F3xx/Include -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Core/helpers" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Core/ramp" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Core/TMC2209" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-ramp

clean-Core-2f-ramp:
	-$(RM) ./Core/ramp/LinearRamp.d ./Core/ramp/LinearRamp.o ./Core/ramp/LinearRamp.su ./Core/ramp/LinearRamp1.d ./Core/ramp/LinearRamp1.o ./Core/ramp/LinearRamp1.su ./Core/ramp/Ramp.d ./Core/ramp/Ramp.o ./Core/ramp/Ramp.su

.PHONY: clean-Core-2f-ramp

