################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/stdperiph/src/at32f415_adc.c \
../src/stdperiph/src/at32f415_can.c \
../src/stdperiph/src/at32f415_cmp.c \
../src/stdperiph/src/at32f415_crc.c \
../src/stdperiph/src/at32f415_crm.c \
../src/stdperiph/src/at32f415_debug.c \
../src/stdperiph/src/at32f415_dma.c \
../src/stdperiph/src/at32f415_ertc.c \
../src/stdperiph/src/at32f415_exint.c \
../src/stdperiph/src/at32f415_flash.c \
../src/stdperiph/src/at32f415_gpio.c \
../src/stdperiph/src/at32f415_i2c.c \
../src/stdperiph/src/at32f415_misc.c \
../src/stdperiph/src/at32f415_pwc.c \
../src/stdperiph/src/at32f415_sdio.c \
../src/stdperiph/src/at32f415_spi.c \
../src/stdperiph/src/at32f415_tmr.c \
../src/stdperiph/src/at32f415_usart.c \
../src/stdperiph/src/at32f415_usb.c \
../src/stdperiph/src/at32f415_wdt.c \
../src/stdperiph/src/at32f415_wwdt.c 

C_DEPS += \
./src/stdperiph/src/at32f415_adc.d \
./src/stdperiph/src/at32f415_can.d \
./src/stdperiph/src/at32f415_cmp.d \
./src/stdperiph/src/at32f415_crc.d \
./src/stdperiph/src/at32f415_crm.d \
./src/stdperiph/src/at32f415_debug.d \
./src/stdperiph/src/at32f415_dma.d \
./src/stdperiph/src/at32f415_ertc.d \
./src/stdperiph/src/at32f415_exint.d \
./src/stdperiph/src/at32f415_flash.d \
./src/stdperiph/src/at32f415_gpio.d \
./src/stdperiph/src/at32f415_i2c.d \
./src/stdperiph/src/at32f415_misc.d \
./src/stdperiph/src/at32f415_pwc.d \
./src/stdperiph/src/at32f415_sdio.d \
./src/stdperiph/src/at32f415_spi.d \
./src/stdperiph/src/at32f415_tmr.d \
./src/stdperiph/src/at32f415_usart.d \
./src/stdperiph/src/at32f415_usb.d \
./src/stdperiph/src/at32f415_wdt.d \
./src/stdperiph/src/at32f415_wwdt.d 

OBJS += \
./src/stdperiph/src/at32f415_adc.o \
./src/stdperiph/src/at32f415_can.o \
./src/stdperiph/src/at32f415_cmp.o \
./src/stdperiph/src/at32f415_crc.o \
./src/stdperiph/src/at32f415_crm.o \
./src/stdperiph/src/at32f415_debug.o \
./src/stdperiph/src/at32f415_dma.o \
./src/stdperiph/src/at32f415_ertc.o \
./src/stdperiph/src/at32f415_exint.o \
./src/stdperiph/src/at32f415_flash.o \
./src/stdperiph/src/at32f415_gpio.o \
./src/stdperiph/src/at32f415_i2c.o \
./src/stdperiph/src/at32f415_misc.o \
./src/stdperiph/src/at32f415_pwc.o \
./src/stdperiph/src/at32f415_sdio.o \
./src/stdperiph/src/at32f415_spi.o \
./src/stdperiph/src/at32f415_tmr.o \
./src/stdperiph/src/at32f415_usart.o \
./src/stdperiph/src/at32f415_usb.o \
./src/stdperiph/src/at32f415_wdt.o \
./src/stdperiph/src/at32f415_wwdt.o 


# Each subdirectory must supply rules for building sources it contributes
src/stdperiph/src/%.o: ../src/stdperiph/src/%.c src/stdperiph/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=soft -munaligned-access -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -Wunused -Wuninitialized -Wall -Wextra -Wshadow -Waggregate-return -Wfloat-equal -g -DAT32F415RCT7 -DUSE_STDPERIPH_DRIVER -I"../src/cmsis" -I"../src/device" -I"../src/stdperiph/inc" -I"../src/RTOS2/Include" -I"../src/RTOS2/RTX/Config" -I"../src/RTOS2/RTX/Include" -I"../src/RTOS2/RTX/Source" -I"../src/rtthread/include" -I"../src/components/rtt" -I"../src/bsp" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


