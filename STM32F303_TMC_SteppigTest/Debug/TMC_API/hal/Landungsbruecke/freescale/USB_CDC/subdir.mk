################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CDC1.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CS1.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Rx1.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Tx1.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB0.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB1.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_class.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.c \
../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.c 

OBJS += \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CDC1.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CS1.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Rx1.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Tx1.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB0.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB1.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_class.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.o \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.o 

C_DEPS += \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CDC1.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CS1.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Rx1.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Tx1.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB0.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB1.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_class.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.d \
./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.d 


# Each subdirectory must supply rules for building sources it contributes
TMC_API/hal/Landungsbruecke/freescale/USB_CDC/%.o TMC_API/hal/Landungsbruecke/freescale/USB_CDC/%.su: ../TMC_API/hal/Landungsbruecke/freescale/USB_CDC/%.c TMC_API/hal/Landungsbruecke/freescale/USB_CDC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC_API-2f-hal-2f-Landungsbruecke-2f-freescale-2f-USB_CDC

clean-TMC_API-2f-hal-2f-Landungsbruecke-2f-freescale-2f-USB_CDC:
	-$(RM) ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CDC1.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CDC1.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CDC1.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CS1.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CS1.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/CS1.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Rx1.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Rx1.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Rx1.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Tx1.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Tx1.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/Tx1.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB0.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB0.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB0.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB1.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB1.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/USB1.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_class.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_class.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_class.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.su ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.d ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.o ./TMC_API/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.su

.PHONY: clean-TMC_API-2f-hal-2f-Landungsbruecke-2f-freescale-2f-USB_CDC

