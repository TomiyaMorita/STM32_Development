################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../tmc/ic/TMC2209/TMC2209.c 

OBJS += \
./tmc/ic/TMC2209/TMC2209.o 

C_DEPS += \
./tmc/ic/TMC2209/TMC2209.d 


# Each subdirectory must supply rules for building sources it contributes
tmc/ic/TMC2209/%.o tmc/ic/TMC2209/%.su: ../tmc/ic/TMC2209/%.c tmc/ic/TMC2209/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I"/Users/moritatomiya/Documents/GitHub/COPREC_RemoteControl_Software/STM/STMF446_ArmRemoteRobot_Test" -I../Core/Inc -I"/Users/moritatomiya/Documents/GitHub/COPREC_RemoteControl_Software/STM/STMF446_ArmRemoteRobot_Test/tmc" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-tmc-2f-ic-2f-TMC2209

clean-tmc-2f-ic-2f-TMC2209:
	-$(RM) ./tmc/ic/TMC2209/TMC2209.d ./tmc/ic/TMC2209/TMC2209.o ./tmc/ic/TMC2209/TMC2209.su

.PHONY: clean-tmc-2f-ic-2f-TMC2209

