################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC-EvalSystem-master/tmc/BLDC.c \
../TMC-EvalSystem-master/tmc/BoardAssignment.c \
../TMC-EvalSystem-master/tmc/EEPROM.c \
../TMC-EvalSystem-master/tmc/IdDetection_Landungsbruecke.c \
../TMC-EvalSystem-master/tmc/IdDetection_Startrampe.c \
../TMC-EvalSystem-master/tmc/RAMDebug.c \
../TMC-EvalSystem-master/tmc/StepDir.c \
../TMC-EvalSystem-master/tmc/TMCL.c \
../TMC-EvalSystem-master/tmc/VitalSignsMonitor.c 

OBJS += \
./TMC-EvalSystem-master/tmc/BLDC.o \
./TMC-EvalSystem-master/tmc/BoardAssignment.o \
./TMC-EvalSystem-master/tmc/EEPROM.o \
./TMC-EvalSystem-master/tmc/IdDetection_Landungsbruecke.o \
./TMC-EvalSystem-master/tmc/IdDetection_Startrampe.o \
./TMC-EvalSystem-master/tmc/RAMDebug.o \
./TMC-EvalSystem-master/tmc/StepDir.o \
./TMC-EvalSystem-master/tmc/TMCL.o \
./TMC-EvalSystem-master/tmc/VitalSignsMonitor.o 

C_DEPS += \
./TMC-EvalSystem-master/tmc/BLDC.d \
./TMC-EvalSystem-master/tmc/BoardAssignment.d \
./TMC-EvalSystem-master/tmc/EEPROM.d \
./TMC-EvalSystem-master/tmc/IdDetection_Landungsbruecke.d \
./TMC-EvalSystem-master/tmc/IdDetection_Startrampe.d \
./TMC-EvalSystem-master/tmc/RAMDebug.d \
./TMC-EvalSystem-master/tmc/StepDir.d \
./TMC-EvalSystem-master/tmc/TMCL.d \
./TMC-EvalSystem-master/tmc/VitalSignsMonitor.d 


# Each subdirectory must supply rules for building sources it contributes
TMC-EvalSystem-master/tmc/%.o TMC-EvalSystem-master/tmc/%.su: ../TMC-EvalSystem-master/tmc/%.c TMC-EvalSystem-master/tmc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC-2d-EvalSystem-2d-master-2f-tmc

clean-TMC-2d-EvalSystem-2d-master-2f-tmc:
	-$(RM) ./TMC-EvalSystem-master/tmc/BLDC.d ./TMC-EvalSystem-master/tmc/BLDC.o ./TMC-EvalSystem-master/tmc/BLDC.su ./TMC-EvalSystem-master/tmc/BoardAssignment.d ./TMC-EvalSystem-master/tmc/BoardAssignment.o ./TMC-EvalSystem-master/tmc/BoardAssignment.su ./TMC-EvalSystem-master/tmc/EEPROM.d ./TMC-EvalSystem-master/tmc/EEPROM.o ./TMC-EvalSystem-master/tmc/EEPROM.su ./TMC-EvalSystem-master/tmc/IdDetection_Landungsbruecke.d ./TMC-EvalSystem-master/tmc/IdDetection_Landungsbruecke.o ./TMC-EvalSystem-master/tmc/IdDetection_Landungsbruecke.su ./TMC-EvalSystem-master/tmc/IdDetection_Startrampe.d ./TMC-EvalSystem-master/tmc/IdDetection_Startrampe.o ./TMC-EvalSystem-master/tmc/IdDetection_Startrampe.su ./TMC-EvalSystem-master/tmc/RAMDebug.d ./TMC-EvalSystem-master/tmc/RAMDebug.o ./TMC-EvalSystem-master/tmc/RAMDebug.su ./TMC-EvalSystem-master/tmc/StepDir.d ./TMC-EvalSystem-master/tmc/StepDir.o ./TMC-EvalSystem-master/tmc/StepDir.su ./TMC-EvalSystem-master/tmc/TMCL.d ./TMC-EvalSystem-master/tmc/TMCL.o ./TMC-EvalSystem-master/tmc/TMCL.su ./TMC-EvalSystem-master/tmc/VitalSignsMonitor.d ./TMC-EvalSystem-master/tmc/VitalSignsMonitor.o ./TMC-EvalSystem-master/tmc/VitalSignsMonitor.su

.PHONY: clean-TMC-2d-EvalSystem-2d-master-2f-tmc

