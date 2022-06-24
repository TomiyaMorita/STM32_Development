################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC_API/tmc/helpers/CRC.c \
../TMC_API/tmc/helpers/Functions.c 

OBJS += \
./TMC_API/tmc/helpers/CRC.o \
./TMC_API/tmc/helpers/Functions.o 

C_DEPS += \
./TMC_API/tmc/helpers/CRC.d \
./TMC_API/tmc/helpers/Functions.d 


# Each subdirectory must supply rules for building sources it contributes
TMC_API/tmc/helpers/%.o TMC_API/tmc/helpers/%.su: ../TMC_API/tmc/helpers/%.c TMC_API/tmc/helpers/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC_API-2f-tmc-2f-helpers

clean-TMC_API-2f-tmc-2f-helpers:
	-$(RM) ./TMC_API/tmc/helpers/CRC.d ./TMC_API/tmc/helpers/CRC.o ./TMC_API/tmc/helpers/CRC.su ./TMC_API/tmc/helpers/Functions.d ./TMC_API/tmc/helpers/Functions.o ./TMC_API/tmc/helpers/Functions.su

.PHONY: clean-TMC_API-2f-tmc-2f-helpers

