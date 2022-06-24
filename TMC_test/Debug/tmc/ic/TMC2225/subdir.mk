################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tmc/ic/TMC2225/TMC2225.c 

OBJS += \
./tmc/ic/TMC2225/TMC2225.o 

C_DEPS += \
./tmc/ic/TMC2225/TMC2225.d 


# Each subdirectory must supply rules for building sources it contributes
tmc/ic/TMC2225/%.o tmc/ic/TMC2225/%.su: ../tmc/ic/TMC2225/%.c tmc/ic/TMC2225/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Device/ST/STM32F3xx/Include -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Include -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/tmc/helpers -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/tmc/ic -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/tmc/ramp -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tmc-2f-ic-2f-TMC2225

clean-tmc-2f-ic-2f-TMC2225:
	-$(RM) ./tmc/ic/TMC2225/TMC2225.d ./tmc/ic/TMC2225/TMC2225.o ./tmc/ic/TMC2225/TMC2225.su

.PHONY: clean-tmc-2f-ic-2f-TMC2225

