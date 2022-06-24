################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC_API/hal/Startrampe/st/system_stm32f2xx.c 

S_UPPER_SRCS += \
../TMC_API/hal/Startrampe/st/startup.S 

OBJS += \
./TMC_API/hal/Startrampe/st/startup.o \
./TMC_API/hal/Startrampe/st/system_stm32f2xx.o 

S_UPPER_DEPS += \
./TMC_API/hal/Startrampe/st/startup.d 

C_DEPS += \
./TMC_API/hal/Startrampe/st/system_stm32f2xx.d 


# Each subdirectory must supply rules for building sources it contributes
TMC_API/hal/Startrampe/st/%.o: ../TMC_API/hal/Startrampe/st/%.S TMC_API/hal/Startrampe/st/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/tmc2209_test/tmc/ic/TMC2209 -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/tmc2209_test/tmc/helpers -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API/hal/Landungsbruecke/freescale" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"
TMC_API/hal/Startrampe/st/%.o TMC_API/hal/Startrampe/st/%.su: ../TMC_API/hal/Startrampe/st/%.c TMC_API/hal/Startrampe/st/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC_API-2f-hal-2f-Startrampe-2f-st

clean-TMC_API-2f-hal-2f-Startrampe-2f-st:
	-$(RM) ./TMC_API/hal/Startrampe/st/startup.d ./TMC_API/hal/Startrampe/st/startup.o ./TMC_API/hal/Startrampe/st/system_stm32f2xx.d ./TMC_API/hal/Startrampe/st/system_stm32f2xx.o ./TMC_API/hal/Startrampe/st/system_stm32f2xx.su

.PHONY: clean-TMC_API-2f-hal-2f-Startrampe-2f-st

