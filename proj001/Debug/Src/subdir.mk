################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/main.c \
../Src/systemconfig.c \
../Src/usbDriver.c \
../Src/usbFramework.c 

OBJS += \
./Src/main.o \
./Src/systemconfig.o \
./Src/usbDriver.o \
./Src/usbFramework.o 

C_DEPS += \
./Src/main.d \
./Src/systemconfig.d \
./Src/usbDriver.d \
./Src/usbFramework.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32F429I_DISC1 -DSTM32 -DSTM32F429ZITx -DSTM32F4 -DSTM32F429xx -c -I../Inc -I"/home/ashwin/Documents/usb/usbDev_workspace/proj001/Inc/CMSIS/Device/ST/STM32F4xx/Include" -I"/home/ashwin/Documents/usb/usbDev_workspace/proj001/Inc/CMSIS/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

