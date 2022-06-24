################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/helpers/CRC.c \
../Core/helpers/Functions.c 

OBJS += \
./Core/helpers/CRC.o \
./Core/helpers/Functions.o 

C_DEPS += \
./Core/helpers/CRC.d \
./Core/helpers/Functions.d 


# Each subdirectory must supply rules for building sources it contributes
Core/helpers/%.o Core/helpers/%.su: ../Core/helpers/%.c Core/helpers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Device/ST/STM32F3xx/Include -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Core/helpers" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Core/ramp" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Core/TMC2209" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-helpers

clean-Core-2f-helpers:
	-$(RM) ./Core/helpers/CRC.d ./Core/helpers/CRC.o ./Core/helpers/CRC.su ./Core/helpers/Functions.d ./Core/helpers/Functions.o ./Core/helpers/Functions.su

.PHONY: clean-Core-2f-helpers

