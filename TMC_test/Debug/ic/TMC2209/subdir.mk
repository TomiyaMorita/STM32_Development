################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
OBJS += \
./ic/TMC2209/TMC2209.o 

C_DEPS += \
./ic/TMC2209/TMC2209.d 


# Each subdirectory must supply rules for building sources it contributes
ic/TMC2209/%.o ic/TMC2209/%.su: ../ic/TMC2209/%.c ic/TMC2209/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Device/ST/STM32F3xx/Include -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/helpers" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/ramp" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/ic/TMC2209" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ic-2f-TMC2209

clean-ic-2f-TMC2209:
	-$(RM) ./ic/TMC2209/TMC2209.d ./ic/TMC2209/TMC2209.o ./ic/TMC2209/TMC2209.su

.PHONY: clean-ic-2f-TMC2209

