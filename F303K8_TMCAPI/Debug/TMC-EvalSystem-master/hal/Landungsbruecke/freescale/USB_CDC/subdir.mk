################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CDC1.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CS1.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Rx1.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Tx1.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB0.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB1.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_class.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.c \
../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.c 

OBJS += \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CDC1.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CS1.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Rx1.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Tx1.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB0.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB1.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_class.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.o \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.o 

C_DEPS += \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CDC1.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CS1.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Rx1.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Tx1.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB0.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB1.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_class.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.d \
./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.d 


# Each subdirectory must supply rules for building sources it contributes
TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/%.o TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/%.su: ../TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/%.c TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I../Core/Inc -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/F303K8_TMCAPI/TMC-EvalSystem-master/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Landungsbruecke-2f-freescale-2f-USB_CDC

clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Landungsbruecke-2f-freescale-2f-USB_CDC:
	-$(RM) ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CDC1.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CDC1.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CDC1.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CS1.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CS1.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/CS1.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Rx1.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Rx1.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Rx1.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Tx1.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Tx1.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/Tx1.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB0.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB0.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB0.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB1.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB1.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/USB1.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_cdc_pstn.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_class.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_class.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_class.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_dci_kinetis.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_descriptor.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_driver.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/usb_framework.su ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.d ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.o ./TMC-EvalSystem-master/hal/Landungsbruecke/freescale/USB_CDC/wdt_kinetis.su

.PHONY: clean-TMC-2d-EvalSystem-2d-master-2f-hal-2f-Landungsbruecke-2f-freescale-2f-USB_CDC

