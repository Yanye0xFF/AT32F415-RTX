################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/components/libc/newlib/cstdlib.c \
../src/components/libc/newlib/musl_libc.c \
../src/components/libc/newlib/syscalls.c 

C_DEPS += \
./src/components/libc/newlib/cstdlib.d \
./src/components/libc/newlib/musl_libc.d \
./src/components/libc/newlib/syscalls.d 

OBJS += \
./src/components/libc/newlib/cstdlib.o \
./src/components/libc/newlib/musl_libc.o \
./src/components/libc/newlib/syscalls.o 


# Each subdirectory must supply rules for building sources it contributes
src/components/libc/newlib/%.o: ../src/components/libc/newlib/%.c src/components/libc/newlib/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=soft -munaligned-access -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -Wunused -Wuninitialized -Wall -Wshadow -Waggregate-return -Wfloat-equal -DAT32F415RCT7 -DUSE_STDPERIPH_DRIVER -I"../src/cmsis" -I"../src/device" -I"../src/stdperiph/inc" -I"../src/RTOS2/Include" -I"../src/RTOS2/RTX/Config" -I"../src/RTOS2/RTX/Include" -I"../src/RTOS2/RTX/Source" -I"../src/rtthread/include" -I"../src/components/rtt" -I"../src/components/finsh" -I"../src/components/libc/newlib" -I"../src/components/libc/cplusplus" -I"../src/components/libc/cplusplus/os" -I"../src/components/spiflash" -I"../src/bsp" -I"../src/bsp/driver" -std=gnu11 -Wimplicit-fallthrough=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


