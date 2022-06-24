################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TMC_API/hal/Startrampe/st/lib/src/misc.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_adc.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_can.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_crc.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_aes.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_des.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_tdes.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dac.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dbgmcu.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dcmi.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dma.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_exti.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_flash.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_fsmc.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_gpio.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_md5.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_sha1.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_i2c.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_it.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_iwdg.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_pwr.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rcc.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rng.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rtc.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_sdio.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_spi.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_syscfg.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_tim.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_usart.c \
../TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_wwdg.c \
../TMC_API/hal/Startrampe/st/lib/src/usb_core.c \
../TMC_API/hal/Startrampe/st/lib/src/usb_dcd.c \
../TMC_API/hal/Startrampe/st/lib/src/usb_dcd_int.c \
../TMC_API/hal/Startrampe/st/lib/src/usbd_cdc_core.c \
../TMC_API/hal/Startrampe/st/lib/src/usbd_core.c \
../TMC_API/hal/Startrampe/st/lib/src/usbd_ioreq.c \
../TMC_API/hal/Startrampe/st/lib/src/usbd_req.c 

OBJS += \
./TMC_API/hal/Startrampe/st/lib/src/misc.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_adc.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_can.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_crc.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_aes.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_des.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_tdes.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dac.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dbgmcu.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dcmi.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dma.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_exti.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_flash.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_fsmc.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_gpio.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_md5.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_sha1.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_i2c.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_it.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_iwdg.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_pwr.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rcc.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rng.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rtc.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_sdio.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_spi.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_syscfg.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_tim.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_usart.o \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_wwdg.o \
./TMC_API/hal/Startrampe/st/lib/src/usb_core.o \
./TMC_API/hal/Startrampe/st/lib/src/usb_dcd.o \
./TMC_API/hal/Startrampe/st/lib/src/usb_dcd_int.o \
./TMC_API/hal/Startrampe/st/lib/src/usbd_cdc_core.o \
./TMC_API/hal/Startrampe/st/lib/src/usbd_core.o \
./TMC_API/hal/Startrampe/st/lib/src/usbd_ioreq.o \
./TMC_API/hal/Startrampe/st/lib/src/usbd_req.o 

C_DEPS += \
./TMC_API/hal/Startrampe/st/lib/src/misc.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_adc.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_can.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_crc.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_aes.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_des.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_tdes.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dac.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dbgmcu.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dcmi.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dma.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_exti.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_flash.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_fsmc.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_gpio.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_md5.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_sha1.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_i2c.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_it.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_iwdg.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_pwr.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rcc.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rng.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rtc.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_sdio.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_spi.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_syscfg.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_tim.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_usart.d \
./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_wwdg.d \
./TMC_API/hal/Startrampe/st/lib/src/usb_core.d \
./TMC_API/hal/Startrampe/st/lib/src/usb_dcd.d \
./TMC_API/hal/Startrampe/st/lib/src/usb_dcd_int.d \
./TMC_API/hal/Startrampe/st/lib/src/usbd_cdc_core.d \
./TMC_API/hal/Startrampe/st/lib/src/usbd_core.d \
./TMC_API/hal/Startrampe/st/lib/src/usbd_ioreq.d \
./TMC_API/hal/Startrampe/st/lib/src/usbd_req.d 


# Each subdirectory must supply rules for building sources it contributes
TMC_API/hal/Startrampe/st/lib/src/%.o TMC_API/hal/Startrampe/st/lib/src/%.su: ../TMC_API/hal/Startrampe/st/lib/src/%.c TMC_API/hal/Startrampe/st/lib/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F303x8 -c -I/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API -I../Core/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc -I../Drivers/STM32F3xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F3xx/Include -I../Drivers/CMSIS/Include -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API" -I"/Users/moritatomiya/STM32CubeIDE/workspace_1.9.0/STM32F303_TMC_SteppigTest/TMC_API/hal/Landungsbruecke/freescale" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-TMC_API-2f-hal-2f-Startrampe-2f-st-2f-lib-2f-src

clean-TMC_API-2f-hal-2f-Startrampe-2f-st-2f-lib-2f-src:
	-$(RM) ./TMC_API/hal/Startrampe/st/lib/src/misc.d ./TMC_API/hal/Startrampe/st/lib/src/misc.o ./TMC_API/hal/Startrampe/st/lib/src/misc.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_adc.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_adc.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_adc.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_can.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_can.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_can.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_crc.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_crc.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_crc.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_aes.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_aes.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_aes.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_des.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_des.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_des.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_tdes.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_tdes.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_cryp_tdes.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dac.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dac.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dac.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dbgmcu.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dbgmcu.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dbgmcu.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dcmi.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dcmi.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dcmi.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dma.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dma.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_dma.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_exti.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_exti.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_exti.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_flash.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_flash.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_flash.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_fsmc.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_fsmc.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_fsmc.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_gpio.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_gpio.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_gpio.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_md5.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_md5.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_md5.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_sha1.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_sha1.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_hash_sha1.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_i2c.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_i2c.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_i2c.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_it.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_it.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_it.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_iwdg.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_iwdg.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_iwdg.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_pwr.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_pwr.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_pwr.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rcc.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rcc.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rcc.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rng.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rng.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rng.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rtc.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rtc.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_rtc.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_sdio.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_sdio.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_sdio.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_spi.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_spi.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_spi.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_syscfg.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_syscfg.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_syscfg.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_tim.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_tim.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_tim.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_usart.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_usart.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_usart.su ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_wwdg.d ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_wwdg.o ./TMC_API/hal/Startrampe/st/lib/src/stm32f2xx_wwdg.su ./TMC_API/hal/Startrampe/st/lib/src/usb_core.d ./TMC_API/hal/Startrampe/st/lib/src/usb_core.o ./TMC_API/hal/Startrampe/st/lib/src/usb_core.su ./TMC_API/hal/Startrampe/st/lib/src/usb_dcd.d ./TMC_API/hal/Startrampe/st/lib/src/usb_dcd.o ./TMC_API/hal/Startrampe/st/lib/src/usb_dcd.su ./TMC_API/hal/Startrampe/st/lib/src/usb_dcd_int.d ./TMC_API/hal/Startrampe/st/lib/src/usb_dcd_int.o ./TMC_API/hal/Startrampe/st/lib/src/usb_dcd_int.su ./TMC_API/hal/Startrampe/st/lib/src/usbd_cdc_core.d ./TMC_API/hal/Startrampe/st/lib/src/usbd_cdc_core.o ./TMC_API/hal/Startrampe/st/lib/src/usbd_cdc_core.su ./TMC_API/hal/Startrampe/st/lib/src/usbd_core.d ./TMC_API/hal/Startrampe/st/lib/src/usbd_core.o ./TMC_API/hal/Startrampe/st/lib/src/usbd_core.su ./TMC_API/hal/Startrampe/st/lib/src/usbd_ioreq.d ./TMC_API/hal/Startrampe/st/lib/src/usbd_ioreq.o
	-$(RM) ./TMC_API/hal/Startrampe/st/lib/src/usbd_ioreq.su ./TMC_API/hal/Startrampe/st/lib/src/usbd_req.d ./TMC_API/hal/Startrampe/st/lib/src/usbd_req.o ./TMC_API/hal/Startrampe/st/lib/src/usbd_req.su

.PHONY: clean-TMC_API-2f-hal-2f-Startrampe-2f-st-2f-lib-2f-src

