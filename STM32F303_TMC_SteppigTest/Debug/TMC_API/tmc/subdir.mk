################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC_API/tmc/BLDC.c \
../TMC_API/tmc/BoardAssignment.c \
../TMC_API/tmc/EEPROM.c \
../TMC_API/tmc/IdDetection_Landungsbruecke.c \
../TMC_API/tmc/IdDetection_Startrampe.c \
../TMC_API/tmc/RAMDebug.c \
../TMC_API/tmc/StepDir.c \
../TMC_API/tmc/TMCL.c \
../TMC_API/tmc/VitalSignsMonitor.c 

OBJS += \
./TMC_API/tmc/BLDC.o \
./TMC_API/tmc/BoardAssignment.o \
./TMC_API/tmc/EEPROM.o \
./TMC_API/tmc/IdDetection_Landungsbruecke.o \
./TMC_API/tmc/IdDetection_Startrampe.o \
./TMC_API/tmc/RAMDebug.o \
./TMC_API/tmc/StepDir.o \
./TMC_API/tmc/TMCL.o \
./TMC_API/tmc/VitalSignsMonitor.o 

C_DEPS += \
./TMC_API/tmc/BLDC.d \
./TMC_API/tmc/BoardAssignment.d \
./TMC_API/tmc/EEPROM.d \
./TMC_API/tmc/IdDetection_Landungsbruecke.d \
./TMC_API/tmc/IdDetection_Startrampe.d \
./TMC_API/tmc/RAMDebug.d \
./TMC_API/tmc/StepDir.d \
./TMC_API/tmc/TMCL.d \
./TMC_API/tmc/VitalSignsMonitor.d 


# Each subdirectory must supply rules for building sources it contributes
TMC_API/tmc/%.o TMC_API/tmc/%.su: ../TMC_API/tmc/%.c TMC_API/tmc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC_API-2f-tmc

clean-TMC_API-2f-tmc:
	-$(RM) ./TMC_API/tmc/BLDC.d ./TMC_API/tmc/BLDC.o ./TMC_API/tmc/BLDC.su ./TMC_API/tmc/BoardAssignment.d ./TMC_API/tmc/BoardAssignment.o ./TMC_API/tmc/BoardAssignment.su ./TMC_API/tmc/EEPROM.d ./TMC_API/tmc/EEPROM.o ./TMC_API/tmc/EEPROM.su ./TMC_API/tmc/IdDetection_Landungsbruecke.d ./TMC_API/tmc/IdDetection_Landungsbruecke.o ./TMC_API/tmc/IdDetection_Landungsbruecke.su ./TMC_API/tmc/IdDetection_Startrampe.d ./TMC_API/tmc/IdDetection_Startrampe.o ./TMC_API/tmc/IdDetection_Startrampe.su ./TMC_API/tmc/RAMDebug.d ./TMC_API/tmc/RAMDebug.o ./TMC_API/tmc/RAMDebug.su ./TMC_API/tmc/StepDir.d ./TMC_API/tmc/StepDir.o ./TMC_API/tmc/StepDir.su ./TMC_API/tmc/TMCL.d ./TMC_API/tmc/TMCL.o ./TMC_API/tmc/TMCL.su ./TMC_API/tmc/VitalSignsMonitor.d ./TMC_API/tmc/VitalSignsMonitor.o ./TMC_API/tmc/VitalSignsMonitor.su

.PHONY: clean-TMC_API-2f-tmc

