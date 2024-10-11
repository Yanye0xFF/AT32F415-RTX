################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/RTOS2/RTX/Source/rtx_delay.c \
../src/RTOS2/RTX/Source/rtx_evflags.c \
../src/RTOS2/RTX/Source/rtx_evr.c \
../src/RTOS2/RTX/Source/rtx_kernel.c \
../src/RTOS2/RTX/Source/rtx_lib.c \
../src/RTOS2/RTX/Source/rtx_memory.c \
../src/RTOS2/RTX/Source/rtx_mempool.c \
../src/RTOS2/RTX/Source/rtx_msgqueue.c \
../src/RTOS2/RTX/Source/rtx_mutex.c \
../src/RTOS2/RTX/Source/rtx_semaphore.c \
../src/RTOS2/RTX/Source/rtx_system.c \
../src/RTOS2/RTX/Source/rtx_thread.c \
../src/RTOS2/RTX/Source/rtx_timer.c 

C_DEPS += \
./src/RTOS2/RTX/Source/rtx_delay.d \
./src/RTOS2/RTX/Source/rtx_evflags.d \
./src/RTOS2/RTX/Source/rtx_evr.d \
./src/RTOS2/RTX/Source/rtx_kernel.d \
./src/RTOS2/RTX/Source/rtx_lib.d \
./src/RTOS2/RTX/Source/rtx_memory.d \
./src/RTOS2/RTX/Source/rtx_mempool.d \
./src/RTOS2/RTX/Source/rtx_msgqueue.d \
./src/RTOS2/RTX/Source/rtx_mutex.d \
./src/RTOS2/RTX/Source/rtx_semaphore.d \
./src/RTOS2/RTX/Source/rtx_system.d \
./src/RTOS2/RTX/Source/rtx_thread.d \
./src/RTOS2/RTX/Source/rtx_timer.d 

OBJS += \
./src/RTOS2/RTX/Source/rtx_delay.o \
./src/RTOS2/RTX/Source/rtx_evflags.o \
./src/RTOS2/RTX/Source/rtx_evr.o \
./src/RTOS2/RTX/Source/rtx_kernel.o \
./src/RTOS2/RTX/Source/rtx_lib.o \
./src/RTOS2/RTX/Source/rtx_memory.o \
./src/RTOS2/RTX/Source/rtx_mempool.o \
./src/RTOS2/RTX/Source/rtx_msgqueue.o \
./src/RTOS2/RTX/Source/rtx_mutex.o \
./src/RTOS2/RTX/Source/rtx_semaphore.o \
./src/RTOS2/RTX/Source/rtx_system.o \
./src/RTOS2/RTX/Source/rtx_thread.o \
./src/RTOS2/RTX/Source/rtx_timer.o 


# Each subdirectory must supply rules for building sources it contributes
src/RTOS2/RTX/Source/%.o: ../src/RTOS2/RTX/Source/%.c src/RTOS2/RTX/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=soft -munaligned-access -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -Wunused -Wuninitialized -Wall -Wshadow -Waggregate-return -Wfloat-equal -DAT32F415RCT7 -DUSE_STDPERIPH_DRIVER -I"../src/cmsis" -I"../src/device" -I"../src/stdperiph/inc" -I"../src/RTOS2/Include" -I"../src/RTOS2/RTX/Config" -I"../src/RTOS2/RTX/Include" -I"../src/RTOS2/RTX/Source" -I"../src/rtthread/include" -I"../src/components/rtt" -I"../src/components/finsh" -I"../src/components/libc/newlib" -I"../src/components/libc/cplusplus" -I"../src/components/libc/cplusplus/os" -I"../src/components/spiflash" -I"../src/bsp" -I"../src/bsp/driver" -std=gnu11 -Wimplicit-fallthrough=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


