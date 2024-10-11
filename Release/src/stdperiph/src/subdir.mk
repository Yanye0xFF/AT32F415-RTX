################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/stdperiph/src/at32f415_adc.c \
../src/stdperiph/src/at32f415_can.c \
../src/stdperiph/src/at32f415_cmp.c \
../src/stdperiph/src/at32f415_crm.c \
../src/stdperiph/src/at32f415_dma.c \
../src/stdperiph/src/at32f415_ertc.c \
../src/stdperiph/src/at32f415_flash.c \
../src/stdperiph/src/at32f415_gpio.c \
../src/stdperiph/src/at32f415_misc.c \
../src/stdperiph/src/at32f415_pwc.c \
../src/stdperiph/src/at32f415_spi.c \
../src/stdperiph/src/at32f415_tmr.c \
../src/stdperiph/src/at32f415_usart.c \
../src/stdperiph/src/at32f415_wdt.c \
../src/stdperiph/src/at32f415_wwdt.c 

C_DEPS += \
./src/stdperiph/src/at32f415_adc.d \
./src/stdperiph/src/at32f415_can.d \
./src/stdperiph/src/at32f415_cmp.d \
./src/stdperiph/src/at32f415_crm.d \
./src/stdperiph/src/at32f415_dma.d \
./src/stdperiph/src/at32f415_ertc.d \
./src/stdperiph/src/at32f415_flash.d \
./src/stdperiph/src/at32f415_gpio.d \
./src/stdperiph/src/at32f415_misc.d \
./src/stdperiph/src/at32f415_pwc.d \
./src/stdperiph/src/at32f415_spi.d \
./src/stdperiph/src/at32f415_tmr.d \
./src/stdperiph/src/at32f415_usart.d \
./src/stdperiph/src/at32f415_wdt.d \
./src/stdperiph/src/at32f415_wwdt.d 

OBJS += \
./src/stdperiph/src/at32f415_adc.o \
./src/stdperiph/src/at32f415_can.o \
./src/stdperiph/src/at32f415_cmp.o \
./src/stdperiph/src/at32f415_crm.o \
./src/stdperiph/src/at32f415_dma.o \
./src/stdperiph/src/at32f415_ertc.o \
./src/stdperiph/src/at32f415_flash.o \
./src/stdperiph/src/at32f415_gpio.o \
./src/stdperiph/src/at32f415_misc.o \
./src/stdperiph/src/at32f415_pwc.o \
./src/stdperiph/src/at32f415_spi.o \
./src/stdperiph/src/at32f415_tmr.o \
./src/stdperiph/src/at32f415_usart.o \
./src/stdperiph/src/at32f415_wdt.o \
./src/stdperiph/src/at32f415_wwdt.o 


# Each subdirectory must supply rules for building sources it contributes
src/stdperiph/src/%.o: ../src/stdperiph/src/%.c src/stdperiph/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=soft -munaligned-access -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -Wunused -Wuninitialized -Wall -Wshadow -Waggregate-return -Wfloat-equal -DAT32F415RCT7 -DUSE_STDPERIPH_DRIVER -I"../src/cmsis" -I"../src/device" -I"../src/stdperiph/inc" -I"../src/RTOS2/Include" -I"../src/RTOS2/RTX/Config" -I"../src/RTOS2/RTX/Include" -I"../src/RTOS2/RTX/Source" -I"../src/rtthread/include" -I"../src/components/rtt" -I"../src/components/finsh" -I"../src/components/libc/newlib" -I"../src/components/libc/cplusplus" -I"../src/components/libc/cplusplus/os" -I"../src/components/spiflash" -I"../src/bsp" -I"../src/bsp/driver" -std=gnu11 -Wimplicit-fallthrough=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


