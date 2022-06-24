################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/Cpu.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/kinetis_sysinit.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/nvic-m4.c 

S_UPPER_SRCS += \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/startup.S 

OBJS += \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/Cpu.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/kinetis_sysinit.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/nvic-m4.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/startup.o 

S_UPPER_DEPS += \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/startup.d 

C_DEPS += \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/Cpu.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/kinetis_sysinit.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/nvic-m4.d 


# Each subdirectory must supply rules for building sources it contributes
TMC-EvalSystem-master/hal/Landungsbruecke/freescale/%.o TMC-EvalSystem-master/hal/Landungsbruecke/freescale/%.su: ../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/%.c TMC-EvalSystem-master/hal/Landungsbruecke/freescale/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
TMC-EvalSystem-master/hal/Landungsbruecke/freescale/%.o: ../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/%.S TMC-EvalSystem-master/hal/Landungsbruecke/freescale/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master/hal/Landungsbruecke/freescale" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Landungsbruecke-2f-freescale

clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Landungsbruecke-2f-freescale:
	-$(RM) ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/Cpu.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/Cpu.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/Cpu.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/kinetis_sysinit.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/kinetis_sysinit.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/kinetis_sysinit.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/nvic-m4.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/nvic-m4.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/nvic-m4.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/startup.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/startup.o

.PHONY: clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Landungsbruecke-2f-freescale

