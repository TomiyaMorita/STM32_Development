################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tmc/helpers/CRC.c \
../tmc/helpers/Functions.c 

OBJS += \
./tmc/helpers/CRC.o \
./tmc/helpers/Functions.o 

C_DEPS += \
./tmc/helpers/CRC.d \
./tmc/helpers/Functions.d 


# Each subdirectory must supply rules for building sources it contributes
tmc/helpers/%.o tmc/helpers/%.su: ../tmc/helpers/%.c tmc/helpers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/tmc/helpers" -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Device/ST/STM32F3xx/Include -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Include -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/tmc/ic/TMC2209 -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Core -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Drivers -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/Drivers/STM32F3xx_HAL_Driver -I../Core/Inc -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tmc-2f-helpers

clean-tmc-2f-helpers:
	-$(RM) ./tmc/helpers/CRC.d ./tmc/helpers/CRC.o ./tmc/helpers/CRC.su ./tmc/helpers/Functions.d ./tmc/helpers/Functions.o ./tmc/helpers/Functions.su

.PHONY: clean-tmc-2f-helpers

