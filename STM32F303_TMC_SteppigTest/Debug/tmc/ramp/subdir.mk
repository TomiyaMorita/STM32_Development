################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tmc/ramp/LinearRamp.c \
../tmc/ramp/LinearRamp1.c \
../tmc/ramp/Ramp.c 

OBJS += \
./tmc/ramp/LinearRamp.o \
./tmc/ramp/LinearRamp1.o \
./tmc/ramp/Ramp.o 

C_DEPS += \
./tmc/ramp/LinearRamp.d \
./tmc/ramp/LinearRamp1.d \
./tmc/ramp/Ramp.d 


# Each subdirectory must supply rules for building sources it contributes
tmc/ramp/%.o tmc/ramp/%.su: ../tmc/ramp/%.c tmc/ramp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/tmc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I../tmc/helpers -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tmc-2f-ramp

clean-tmc-2f-ramp:
	-$(RM) ./tmc/ramp/LinearRamp.d ./tmc/ramp/LinearRamp.o ./tmc/ramp/LinearRamp.su ./tmc/ramp/LinearRamp1.d ./tmc/ramp/LinearRamp1.o ./tmc/ramp/LinearRamp1.su ./tmc/ramp/Ramp.d ./tmc/ramp/Ramp.o ./tmc/ramp/Ramp.su

.PHONY: clean-tmc-2f-ramp

