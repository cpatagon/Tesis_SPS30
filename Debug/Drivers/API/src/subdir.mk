################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../Drivers/API/src/shdlc.c \
../Drivers/API/src/shdlc_frame.c \
../Drivers/API/src/sps30.c

OBJS += \
./Drivers/API/src/shdlc.o \
./Drivers/API/src/shdlc_frame.o \
./Drivers/API/src/sps30.o

C_DEPS += \
./Drivers/API/src/shdlc.d \
./Drivers/API/src/shdlc_frame.d \
./Drivers/API/src/sps30.d


# Each subdirectory must supply rules for building sources it contributes
Drivers/API/src/%.o Drivers/API/src/%.su Drivers/API/src/%.cyclo: ../Drivers/API/src/%.c Drivers/API/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/STM32/Tesis_DHT22/Drivers/API/Inc" -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/STM32/Tesis_DHT22/Drivers/API" -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -u _scanf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-API-2f-src

clean-Drivers-2f-API-2f-src:
	-$(RM) ./Drivers/API/src/shdlc.cyclo ./Drivers/API/src/shdlc.d ./Drivers/API/src/shdlc.o ./Drivers/API/src/shdlc.su ./Drivers/API/src/shdlc_frame.cyclo ./Drivers/API/src/shdlc_frame.d ./Drivers/API/src/shdlc_frame.o ./Drivers/API/src/shdlc_frame.su ./Drivers/API/src/sps30.cyclo ./Drivers/API/src/sps30.d ./Drivers/API/src/sps30.o ./Drivers/API/src/sps30.su

.PHONY: clean-Drivers-2f-API-2f-src
