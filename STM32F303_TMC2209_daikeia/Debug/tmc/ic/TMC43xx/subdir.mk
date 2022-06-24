################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tmc/ic/TMC43xx/TMC43xx.c 

OBJS += \
./tmc/ic/TMC43xx/TMC43xx.o 

C_DEPS += \
./tmc/ic/TMC43xx/TMC43xx.d 


# Each subdirectory must supply rules for building sources it contributes
tmc/ic/TMC43xx/%.o tmc/ic/TMC43xx/%.su: ../tmc/ic/TMC43xx/%.c tmc/ic/TMC43xx/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../STM32F303_TMC_SteppingTest_notLib -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tmc-2f-ic-2f-TMC43xx

clean-tmc-2f-ic-2f-TMC43xx:
	-$(RM) ./tmc/ic/TMC43xx/TMC43xx.d ./tmc/ic/TMC43xx/TMC43xx.o ./tmc/ic/TMC43xx/TMC43xx.su

.PHONY: clean-tmc-2f-ic-2f-TMC43xx

