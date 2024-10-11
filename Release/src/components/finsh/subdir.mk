################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/components/finsh/cmd.c \
../src/components/finsh/finsh_port.c \
../src/components/finsh/msh.c \
../src/components/finsh/msh_parse.c \
../src/components/finsh/shell.c 

C_DEPS += \
./src/components/finsh/cmd.d \
./src/components/finsh/finsh_port.d \
./src/components/finsh/msh.d \
./src/components/finsh/msh_parse.d \
./src/components/finsh/shell.d 

OBJS += \
./src/components/finsh/cmd.o \
./src/components/finsh/finsh_port.o \
./src/components/finsh/msh.o \
./src/components/finsh/msh_parse.o \
./src/components/finsh/shell.o 


# Each subdirectory must supply rules for building sources it contributes
src/components/finsh/%.o: ../src/components/finsh/%.c src/components/finsh/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -march=armv7e-m -mthumb -mlittle-endian -mfloat-abi=soft -munaligned-access -O2 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -ffreestanding -Wunused -Wuninitialized -Wall -Wshadow -Waggregate-return -Wfloat-equal -DAT32F415RCT7 -DUSE_STDPERIPH_DRIVER -I"../src/cmsis" -I"../src/device" -I"../src/stdperiph/inc" -I"../src/RTOS2/Include" -I"../src/RTOS2/RTX/Config" -I"../src/RTOS2/RTX/Include" -I"../src/RTOS2/RTX/Source" -I"../src/rtthread/include" -I"../src/components/rtt" -I"../src/components/finsh" -I"../src/components/libc/newlib" -I"../src/components/libc/cplusplus" -I"../src/components/libc/cplusplus/os" -I"../src/components/spiflash" -I"../src/bsp" -I"../src/bsp/driver" -std=gnu11 -Wimplicit-fallthrough=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


