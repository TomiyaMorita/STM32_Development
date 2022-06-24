################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC_API/hal/Startrampe/tmc/ADCs.c \
../TMC_API/hal/Startrampe/tmc/HAL.c \
../TMC_API/hal/Startrampe/tmc/IOMap.c \
../TMC_API/hal/Startrampe/tmc/IOs.c \
../TMC_API/hal/Startrampe/tmc/LEDs.c \
../TMC_API/hal/Startrampe/tmc/RS232.c \
../TMC_API/hal/Startrampe/tmc/RXTX.c \
../TMC_API/hal/Startrampe/tmc/SPI.c \
../TMC_API/hal/Startrampe/tmc/SysTick.c \
../TMC_API/hal/Startrampe/tmc/Timer.c \
../TMC_API/hal/Startrampe/tmc/UART.c \
../TMC_API/hal/Startrampe/tmc/USB.c \
../TMC_API/hal/Startrampe/tmc/WLAN.c 

OBJS += \
./TMC_API/hal/Startrampe/tmc/ADCs.o \
./TMC_API/hal/Startrampe/tmc/HAL.o \
./TMC_API/hal/Startrampe/tmc/IOMap.o \
./TMC_API/hal/Startrampe/tmc/IOs.o \
./TMC_API/hal/Startrampe/tmc/LEDs.o \
./TMC_API/hal/Startrampe/tmc/RS232.o \
./TMC_API/hal/Startrampe/tmc/RXTX.o \
./TMC_API/hal/Startrampe/tmc/SPI.o \
./TMC_API/hal/Startrampe/tmc/SysTick.o \
./TMC_API/hal/Startrampe/tmc/Timer.o \
./TMC_API/hal/Startrampe/tmc/UART.o \
./TMC_API/hal/Startrampe/tmc/USB.o \
./TMC_API/hal/Startrampe/tmc/WLAN.o 

C_DEPS += \
./TMC_API/hal/Startrampe/tmc/ADCs.d \
./TMC_API/hal/Startrampe/tmc/HAL.d \
./TMC_API/hal/Startrampe/tmc/IOMap.d \
./TMC_API/hal/Startrampe/tmc/IOs.d \
./TMC_API/hal/Startrampe/tmc/LEDs.d \
./TMC_API/hal/Startrampe/tmc/RS232.d \
./TMC_API/hal/Startrampe/tmc/RXTX.d \
./TMC_API/hal/Startrampe/tmc/SPI.d \
./TMC_API/hal/Startrampe/tmc/SysTick.d \
./TMC_API/hal/Startrampe/tmc/Timer.d \
./TMC_API/hal/Startrampe/tmc/UART.d \
./TMC_API/hal/Startrampe/tmc/USB.d \
./TMC_API/hal/Startrampe/tmc/WLAN.d 


# Each subdirectory must supply rules for building sources it contributes
TMC_API/hal/Startrampe/tmc/%.o TMC_API/hal/Startrampe/tmc/%.su: ../TMC_API/hal/Startrampe/tmc/%.c TMC_API/hal/Startrampe/tmc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC_API-2f-hal-2f-Startrampe-2f-tmc

clean-TMC_API-2f-hal-2f-Startrampe-2f-tmc:
	-$(RM) ./TMC_API/hal/Startrampe/tmc/ADCs.d ./TMC_API/hal/Startrampe/tmc/ADCs.o ./TMC_API/hal/Startrampe/tmc/ADCs.su ./TMC_API/hal/Startrampe/tmc/HAL.d ./TMC_API/hal/Startrampe/tmc/HAL.o ./TMC_API/hal/Startrampe/tmc/HAL.su ./TMC_API/hal/Startrampe/tmc/IOMap.d ./TMC_API/hal/Startrampe/tmc/IOMap.o ./TMC_API/hal/Startrampe/tmc/IOMap.su ./TMC_API/hal/Startrampe/tmc/IOs.d ./TMC_API/hal/Startrampe/tmc/IOs.o ./TMC_API/hal/Startrampe/tmc/IOs.su ./TMC_API/hal/Startrampe/tmc/LEDs.d ./TMC_API/hal/Startrampe/tmc/LEDs.o ./TMC_API/hal/Startrampe/tmc/LEDs.su ./TMC_API/hal/Startrampe/tmc/RS232.d ./TMC_API/hal/Startrampe/tmc/RS232.o ./TMC_API/hal/Startrampe/tmc/RS232.su ./TMC_API/hal/Startrampe/tmc/RXTX.d ./TMC_API/hal/Startrampe/tmc/RXTX.o ./TMC_API/hal/Startrampe/tmc/RXTX.su ./TMC_API/hal/Startrampe/tmc/SPI.d ./TMC_API/hal/Startrampe/tmc/SPI.o ./TMC_API/hal/Startrampe/tmc/SPI.su ./TMC_API/hal/Startrampe/tmc/SysTick.d ./TMC_API/hal/Startrampe/tmc/SysTick.o ./TMC_API/hal/Startrampe/tmc/SysTick.su ./TMC_API/hal/Startrampe/tmc/Timer.d ./TMC_API/hal/Startrampe/tmc/Timer.o ./TMC_API/hal/Startrampe/tmc/Timer.su ./TMC_API/hal/Startrampe/tmc/UART.d ./TMC_API/hal/Startrampe/tmc/UART.o ./TMC_API/hal/Startrampe/tmc/UART.su ./TMC_API/hal/Startrampe/tmc/USB.d ./TMC_API/hal/Startrampe/tmc/USB.o ./TMC_API/hal/Startrampe/tmc/USB.su ./TMC_API/hal/Startrampe/tmc/WLAN.d ./TMC_API/hal/Startrampe/tmc/WLAN.o ./TMC_API/hal/Startrampe/tmc/WLAN.su

.PHONY: clean-TMC_API-2f-hal-2f-Startrampe-2f-tmc

