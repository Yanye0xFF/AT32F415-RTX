################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/components/littlefs/lfs.c \
../src/components/littlefs/lfs_posix.c \
../src/components/littlefs/lfs_util.c 

C_DEPS += \
./src/components/littlefs/lfs.d \
./src/components/littlefs/lfs_posix.d \
./src/components/littlefs/lfs_util.d 

OBJS += \
./src/components/littlefs/lfs.o \
./src/components/littlefs/lfs_posix.o \
./src/components/littlefs/lfs_util.o 


# Each subdirectory must supply rules for building sources it contributes
src/components/littlefs/%.o: ../src/components/littlefs/%.c src/components/littlefs/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=soft -munaligned-access -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -Wunused -Wuninitialized -Wall -Wshadow -Waggregate-return -Wfloat-equal -DAT32F415RCT7 -DUSE_STDPERIPH_DRIVER -I"../src/cmsis" -I"../src/device" -I"../src/stdperiph/inc" -I"../src/RTOS2/Include" -I"../src/RTOS2/RTX/Config" -I"../src/RTOS2/RTX/Include" -I"../src/RTOS2/RTX/Source" -I"../src/rtthread/include" -I"../src/components/rtt" -I"../src/components/finsh" -I"../src/components/libc/newlib" -I"../src/components/libc/cplusplus" -I"../src/components/libc/cplusplus/os" -I"../src/components/spiflash" -I"../src/bsp" -I"../src/bsp/driver" -std=gnu11 -Wimplicit-fallthrough=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


