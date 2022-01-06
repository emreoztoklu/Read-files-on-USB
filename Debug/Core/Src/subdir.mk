################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/File_Handling.c \
../Core/Src/STM_flash.c \
../Core/Src/fifo.c \
../Core/Src/main.c \
../Core/Src/retarget.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/File_Handling.o \
./Core/Src/STM_flash.o \
./Core/Src/fifo.o \
./Core/Src/main.o \
./Core/Src/retarget.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/File_Handling.d \
./Core/Src/STM_flash.d \
./Core/Src/fifo.d \
./Core/Src/main.d \
./Core/Src/retarget.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F407xx -c -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/File_Handling.d ./Core/Src/File_Handling.o ./Core/Src/STM_flash.d ./Core/Src/STM_flash.o ./Core/Src/fifo.d ./Core/Src/fifo.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/retarget.d ./Core/Src/retarget.o ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o

.PHONY: clean-Core-2f-Src

