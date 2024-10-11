################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/components/libc/cplusplus/os/cxx_Mutex.cpp \
../src/components/libc/cplusplus/os/cxx_Semaphore.cpp 

OBJS += \
./src/components/libc/cplusplus/os/cxx_Mutex.o \
./src/components/libc/cplusplus/os/cxx_Semaphore.o 

CPP_DEPS += \
./src/components/libc/cplusplus/os/cxx_Mutex.d \
./src/components/libc/cplusplus/os/cxx_Semaphore.d 


# Each subdirectory must supply rules for building sources it contributes
src/components/libc/cplusplus/os/%.o: ../src/components/libc/cplusplus/os/%.cpp src/components/libc/cplusplus/os/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C++ Compiler'
	arm-none-eabi-g++ -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=soft -munaligned-access -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -Wunused -Wuninitialized -Wall -Wshadow -Waggregate-return -Wfloat-equal -DAT32F415RCT7 -DUSE_STDPERIPH_DRIVER -I"../src/cmsis" -I"../src/device" -I"../src/stdperiph/inc" -I"../src/RTOS2/Include" -I"../src/RTOS2/RTX/Include" -I"../src/rtthread/include" -I"../src/components/libc/newlib" -I"../src/components/libc/cplusplus" -I"../src/components/libc/cplusplus/os" -I"../src/components/rtt" -I"../src/components/finsh" -I"../src/components/spiflash" -I"../src/components/littlefs" -I"../src/components/ipc" -std=gnu++14 -fabi-version=0 -fno-exceptions -fno-rtti -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


