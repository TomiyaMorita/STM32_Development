################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ramp/LinearRamp.c \
../ramp/LinearRamp1.c \
../ramp/Ramp.c 

OBJS += \
./ramp/LinearRamp.o \
./ramp/LinearRamp1.o \
./ramp/Ramp.o 

C_DEPS += \
./ramp/LinearRamp.d \
./ramp/LinearRamp1.d \
./ramp/Ramp.d 


# Each subdirectory must supply rules for building sources it contributes
ramp/%.o ramp/%.su: ../ramp/%.c ramp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Device/ST/STM32F3xx/Include -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/helpers" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/ramp" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/ic/TMC2209" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ramp

clean-ramp:
	-$(RM) ./ramp/LinearRamp.d ./ramp/LinearRamp.o ./ramp/LinearRamp.su ./ramp/LinearRamp1.d ./ramp/LinearRamp1.o ./ramp/LinearRamp1.su ./ramp/Ramp.d ./ramp/Ramp.o ./ramp/Ramp.su

.PHONY: clean-ramp

