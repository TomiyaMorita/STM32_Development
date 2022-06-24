################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC-EvalSystem-master/hal/Startrampe/tmc/HAL.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/IOMap.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/IOs.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/LEDs.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/RS232.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/RXTX.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/SPI.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/SysTick.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/Timer.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/UART.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/USB.c \
../TMC-EvalSystem-master/hal/Startrampe/tmc/WLAN.c 

OBJS += \
./TMC-EvalSystem-master/hal/Startrampe/tmc/HAL.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/IOMap.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/IOs.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/LEDs.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/RS232.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/RXTX.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/SPI.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/SysTick.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/Timer.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/UART.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/USB.o \
./TMC-EvalSystem-master/hal/Startrampe/tmc/WLAN.o 

C_DEPS += \
./TMC-EvalSystem-master/hal/Startrampe/tmc/HAL.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/IOMap.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/IOs.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/LEDs.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/RS232.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/RXTX.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/SPI.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/SysTick.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/Timer.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/UART.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/USB.d \
./TMC-EvalSystem-master/hal/Startrampe/tmc/WLAN.d 


# Each subdirectory must supply rules for building sources it contributes
TMC-EvalSystem-master/hal/Startrampe/tmc/%.o TMC-EvalSystem-master/hal/Startrampe/tmc/%.su: ../TMC-EvalSystem-master/hal/Startrampe/tmc/%.c TMC-EvalSystem-master/hal/Startrampe/tmc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Startrampe-2f-tmc

clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Startrampe-2f-tmc:
	-$(RM) ./TMC-EvalSystem-master/hal/Startrampe/tmc/HAL.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/HAL.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/HAL.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/IOMap.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/IOMap.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/IOMap.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/IOs.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/IOs.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/IOs.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/LEDs.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/LEDs.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/LEDs.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/RS232.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/RS232.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/RS232.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/RXTX.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/RXTX.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/RXTX.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/SPI.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/SPI.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/SPI.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/SysTick.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/SysTick.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/SysTick.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/Timer.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/Timer.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/Timer.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/UART.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/UART.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/UART.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/USB.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/USB.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/USB.su ./TMC-EvalSystem-master/hal/Startrampe/tmc/WLAN.d ./TMC-EvalSystem-master/hal/Startrampe/tmc/WLAN.o ./TMC-EvalSystem-master/hal/Startrampe/tmc/WLAN.su

.PHONY: clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Startrampe-2f-tmc

