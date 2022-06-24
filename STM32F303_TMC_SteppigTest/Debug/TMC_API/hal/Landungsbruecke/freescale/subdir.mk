################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC_API/hal/Landungsbruecke/freescale/Cpu.c \
../TMC_API/hal/Landungsbruecke/freescale/kinetis_sysinit.c \
../TMC_API/hal/Landungsbruecke/freescale/nvic-m4.c 

S_UPPER_SRCS += \
../TMC_API/hal/Landungsbruecke/freescale/startup.S 

OBJS += \
./TMC_API/hal/Landungsbruecke/freescale/Cpu.o \
./TMC_API/hal/Landungsbruecke/freescale/kinetis_sysinit.o \
./TMC_API/hal/Landungsbruecke/freescale/nvic-m4.o \
./TMC_API/hal/Landungsbruecke/freescale/startup.o 

S_UPPER_DEPS += \
./TMC_API/hal/Landungsbruecke/freescale/startup.d 

C_DEPS += \
./TMC_API/hal/Landungsbruecke/freescale/Cpu.d \
./TMC_API/hal/Landungsbruecke/freescale/kinetis_sysinit.d \
./TMC_API/hal/Landungsbruecke/freescale/nvic-m4.d 


# Each subdirectory must supply rules for building sources it contributes
TMC_API/hal/Landungsbruecke/freescale/%.o TMC_API/hal/Landungsbruecke/freescale/%.su: ../TMC_API/hal/Landungsbruecke/freescale/%.c TMC_API/hal/Landungsbruecke/freescale/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
TMC_API/hal/Landungsbruecke/freescale/%.o: ../TMC_API/hal/Landungsbruecke/freescale/%.S TMC_API/hal/Landungsbruecke/freescale/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/tmc2209_test/tmc/ic/TMC2209 -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/tmc2209_test/tmc/helpers -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API/hal/Landungsbruecke/freescale" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-TMC_API-2f-hal-2f-Landungsbruecke-2f-freescale

clean-TMC_API-2f-hal-2f-Landungsbruecke-2f-freescale:
	-$(RM) ./TMC_API/hal/Landungsbruecke/freescale/Cpu.d ./TMC_API/hal/Landungsbruecke/freescale/Cpu.o ./TMC_API/hal/Landungsbruecke/freescale/Cpu.su ./TMC_API/hal/Landungsbruecke/freescale/kinetis_sysinit.d ./TMC_API/hal/Landungsbruecke/freescale/kinetis_sysinit.o ./TMC_API/hal/Landungsbruecke/freescale/kinetis_sysinit.su ./TMC_API/hal/Landungsbruecke/freescale/nvic-m4.d ./TMC_API/hal/Landungsbruecke/freescale/nvic-m4.o ./TMC_API/hal/Landungsbruecke/freescale/nvic-m4.su ./TMC_API/hal/Landungsbruecke/freescale/startup.d ./TMC_API/hal/Landungsbruecke/freescale/startup.o

.PHONY: clean-TMC_API-2f-hal-2f-Landungsbruecke-2f-freescale

