################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/ADCs.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/HAL.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOMap.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOs.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/LEDs.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RS232.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RXTX.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SPI.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SysTick.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/Timer.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/UART.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/USB.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/WLAN.c 

OBJS += \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/ADCs.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/HAL.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOMap.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOs.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/LEDs.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RS232.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RXTX.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SPI.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SysTick.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/Timer.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/UART.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/USB.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/WLAN.o 

C_DEPS += \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/ADCs.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/HAL.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOMap.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOs.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/LEDs.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RS232.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RXTX.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SPI.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SysTick.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/Timer.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/UART.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/USB.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/WLAN.d 


# Each subdirectory must supply rules for building sources it contributes
TMC-EvalSystem-master/hal/Landungsbruecke/tmc/%.o TMC-EvalSystem-master/hal/Landungsbruecke/tmc/%.su: ../TMC-EvalSystem-master/hal/Landungsbruecke/tmc/%.c TMC-EvalSystem-master/hal/Landungsbruecke/tmc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Landungsbruecke-2f-tmc

clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Landungsbruecke-2f-tmc:
	-$(RM) ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/ADCs.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/ADCs.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/ADCs.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/HAL.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/HAL.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/HAL.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOMap.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOMap.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOMap.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOs.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOs.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/IOs.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/LEDs.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/LEDs.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/LEDs.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RS232.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RS232.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RS232.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RXTX.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RXTX.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/RXTX.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SPI.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SPI.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SPI.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SysTick.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SysTick.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/SysTick.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/Timer.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/Timer.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/Timer.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/UART.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/UART.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/UART.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/USB.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/USB.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/USB.su ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/WLAN.d ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/WLAN.o ./TMC-EvalSystem-master/hal/Landungsbruecke/tmc/WLAN.su

.PHONY: clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Landungsbruecke-2f-tmc

