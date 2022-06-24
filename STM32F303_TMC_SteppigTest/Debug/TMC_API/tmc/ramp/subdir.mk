################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC_API/tmc/ramp/LinearRamp.c \
../TMC_API/tmc/ramp/LinearRamp1.c \
../TMC_API/tmc/ramp/Ramp.c 

OBJS += \
./TMC_API/tmc/ramp/LinearRamp.o \
./TMC_API/tmc/ramp/LinearRamp1.o \
./TMC_API/tmc/ramp/Ramp.o 

C_DEPS += \
./TMC_API/tmc/ramp/LinearRamp.d \
./TMC_API/tmc/ramp/LinearRamp1.d \
./TMC_API/tmc/ramp/Ramp.d 


# Each subdirectory must supply rules for building sources it contributes
TMC_API/tmc/ramp/%.o TMC_API/tmc/ramp/%.su: ../TMC_API/tmc/ramp/%.c TMC_API/tmc/ramp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC_API-2f-tmc-2f-ramp

clean-TMC_API-2f-tmc-2f-ramp:
	-$(RM) ./TMC_API/tmc/ramp/LinearRamp.d ./TMC_API/tmc/ramp/LinearRamp.o ./TMC_API/tmc/ramp/LinearRamp.su ./TMC_API/tmc/ramp/LinearRamp1.d ./TMC_API/tmc/ramp/LinearRamp1.o ./TMC_API/tmc/ramp/LinearRamp1.su ./TMC_API/tmc/ramp/Ramp.d ./TMC_API/tmc/ramp/Ramp.o ./TMC_API/tmc/ramp/Ramp.su

.PHONY: clean-TMC_API-2f-tmc-2f-ramp

