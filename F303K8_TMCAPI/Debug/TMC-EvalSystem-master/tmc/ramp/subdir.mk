################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC-EvalSystem-master/tmc/ramp/LinearRamp.c \
../TMC-EvalSystem-master/tmc/ramp/LinearRamp1.c \
../TMC-EvalSystem-master/tmc/ramp/Ramp.c 

OBJS += \
./TMC-EvalSystem-master/tmc/ramp/LinearRamp.o \
./TMC-EvalSystem-master/tmc/ramp/LinearRamp1.o \
./TMC-EvalSystem-master/tmc/ramp/Ramp.o 

C_DEPS += \
./TMC-EvalSystem-master/tmc/ramp/LinearRamp.d \
./TMC-EvalSystem-master/tmc/ramp/LinearRamp1.d \
./TMC-EvalSystem-master/tmc/ramp/Ramp.d 


# Each subdirectory must supply rules for building sources it contributes
TMC-EvalSystem-master/tmc/ramp/%.o TMC-EvalSystem-master/tmc/ramp/%.su: ../TMC-EvalSystem-master/tmc/ramp/%.c TMC-EvalSystem-master/tmc/ramp/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC-2d-EvalSystem-2d-master-2f-tmc-2f-ramp

clean-TMC-2d-EvalSystem-2d-master-2f-tmc-2f-ramp:
	-$(RM) ./TMC-EvalSystem-master/tmc/ramp/LinearRamp.d ./TMC-EvalSystem-master/tmc/ramp/LinearRamp.o ./TMC-EvalSystem-master/tmc/ramp/LinearRamp.su ./TMC-EvalSystem-master/tmc/ramp/LinearRamp1.d ./TMC-EvalSystem-master/tmc/ramp/LinearRamp1.o ./TMC-EvalSystem-master/tmc/ramp/LinearRamp1.su ./TMC-EvalSystem-master/tmc/ramp/Ramp.d ./TMC-EvalSystem-master/tmc/ramp/Ramp.o ./TMC-EvalSystem-master/tmc/ramp/Ramp.su

.PHONY: clean-TMC-2d-EvalSystem-2d-master-2f-tmc-2f-ramp

