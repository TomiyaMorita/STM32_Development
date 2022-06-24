################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC-EvalSystem-master/tmc/ic/TMC2209/TMC2209.c 

OBJS += \
./TMC-EvalSystem-master/tmc/ic/TMC2209/TMC2209.o 

C_DEPS += \
./TMC-EvalSystem-master/tmc/ic/TMC2209/TMC2209.d 


# Each subdirectory must supply rules for building sources it contributes
TMC-EvalSystem-master/tmc/ic/TMC2209/%.o TMC-EvalSystem-master/tmc/ic/TMC2209/%.su: ../TMC-EvalSystem-master/tmc/ic/TMC2209/%.c TMC-EvalSystem-master/tmc/ic/TMC2209/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC-2d-EvalSystem-2d-master-2f-tmc-2f-ic-2f-TMC2209

clean-TMC-2d-EvalSystem-2d-master-2f-tmc-2f-ic-2f-TMC2209:
	-$(RM) ./TMC-EvalSystem-master/tmc/ic/TMC2209/TMC2209.d ./TMC-EvalSystem-master/tmc/ic/TMC2209/TMC2209.o ./TMC-EvalSystem-master/tmc/ic/TMC2209/TMC2209.su

.PHONY: clean-TMC-2d-EvalSystem-2d-master-2f-tmc-2f-ic-2f-TMC2209

