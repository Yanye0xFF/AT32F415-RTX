################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../src/libcpu/at32f415_crt0.S \
../src/libcpu/exception_gcc.S 

C_SRCS += \
../src/libcpu/cpuport.c 

C_DEPS += \
./src/libcpu/cpuport.d 

OBJS += \
./src/libcpu/at32f415_crt0.o \
./src/libcpu/cpuport.o \
./src/libcpu/exception_gcc.o 

S_UPPER_DEPS += \
./src/libcpu/at32f415_crt0.d \
./src/libcpu/exception_gcc.d 


# Each subdirectory must supply rules for building sources it contributes
src/libcpu/%.o: ../src/libcpu/%.S src/libcpu/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross Assembler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=soft -munaligned-access -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -Wunused -Wuninitialized -Wall -Wextra -Wshadow -Waggregate-return -Wfloat-equal -g -x assembler-with-cpp -DAT32F415RCT7 -DUSE_STDPERIPH_DRIVER -I"../src/device" -I"../src/RTOS2/RTX/Include" -I"../src/RTOS2/RTX/Config" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/libcpu/%.o: ../src/libcpu/%.c src/libcpu/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=soft -munaligned-access -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -Wunused -Wuninitialized -Wall -Wextra -Wshadow -Waggregate-return -Wfloat-equal -g -DAT32F415RCT7 -DUSE_STDPERIPH_DRIVER -I"../src/cmsis" -I"../src/device" -I"../src/stdperiph/inc" -I"../src/RTOS2/Include" -I"../src/RTOS2/RTX/Config" -I"../src/RTOS2/RTX/Include" -I"../src/RTOS2/RTX/Source" -I"../src/rtthread/include" -I"../src/components/rtt" -I"../src/bsp" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


