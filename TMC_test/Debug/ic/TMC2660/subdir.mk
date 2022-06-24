################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ic/TMC2660/TMC2660.c 

OBJS += \
./ic/TMC2660/TMC2660.o 

C_DEPS += \
./ic/TMC2660/TMC2660.d 


# Each subdirectory must supply rules for building sources it contributes
ic/TMC2660/%.o ic/TMC2660/%.su: ../ic/TMC2660/%.c ic/TMC2660/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Device/ST/STM32F3xx/Include -I/Users/moritatomiya/STM32Cube/Repository/STM32Cube_FW_F3_V1.11.3/Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/helpers" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/ramp" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/TMC_test/ic/TMC2209" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-ic-2f-TMC2660

clean-ic-2f-TMC2660:
	-$(RM) ./ic/TMC2660/TMC2660.d ./ic/TMC2660/TMC2660.o ./ic/TMC2660/TMC2660.su

.PHONY: clean-ic-2f-TMC2660

