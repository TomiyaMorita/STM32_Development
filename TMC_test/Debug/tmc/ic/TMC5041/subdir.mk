################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tmc/ic/TMC5041/TMC5041.c 

OBJS += \
./tmc/ic/TMC5041/TMC5041.o 

C_DEPS += \
./tmc/ic/TMC5041/TMC5041.d 


# Each subdirectory must supply rules for building sources it contributes
tmc/ic/TMC5041/%.o tmc/ic/TMC5041/%.su: ../tmc/ic/TMC5041/%.c tmc/ic/TMC5041/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Device/ST/STM32F3xx/Include -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Include -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/tmc/helpers -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/tmc/ic -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/tmc/ramp -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tmc-2f-ic-2f-TMC5041

clean-tmc-2f-ic-2f-TMC5041:
	-$(RM) ./tmc/ic/TMC5041/TMC5041.d ./tmc/ic/TMC5041/TMC5041.o ./tmc/ic/TMC5041/TMC5041.su

.PHONY: clean-tmc-2f-ic-2f-TMC5041

