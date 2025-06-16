################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../Tests/stubs/fatfs_stub.c \
../Tests/stubs/microSD_utils.c \
../Tests/stubs/time_rtc.c

OBJS += \
./Tests/stubs/fatfs_stub.o \
./Tests/stubs/microSD_utils.o \
./Tests/stubs/time_rtc.o

C_DEPS += \
./Tests/stubs/fatfs_stub.d \
./Tests/stubs/microSD_utils.d \
./Tests/stubs/time_rtc.d


# Each subdirectory must supply rules for building sources it contributes
Tests/stubs/%.o Tests/stubs/%.su Tests/stubs/%.cyclo: ../Tests/stubs/%.c Tests/stubs/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUNIT_TESTING -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/STM32F4xx_HAL_Driver/Inc -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/CMSIS/Device/ST/STM32F4xx/Include -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/CMSIS/Include -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs" -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs/Inc" -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs/Src" -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs/Config" -I../FATFS/Target -I../FATFS/App -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Middlewares/Third_Party/FatFs/src -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/Tests" -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -u _scanf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Tests-2f-stubs

clean-Tests-2f-stubs:
	-$(RM) ./Tests/stubs/fatfs_stub.cyclo ./Tests/stubs/fatfs_stub.d ./Tests/stubs/fatfs_stub.o ./Tests/stubs/fatfs_stub.su ./Tests/stubs/microSD_utils.cyclo ./Tests/stubs/microSD_utils.d ./Tests/stubs/microSD_utils.o ./Tests/stubs/microSD_utils.su ./Tests/stubs/time_rtc.cyclo ./Tests/stubs/time_rtc.d ./Tests/stubs/time_rtc.o ./Tests/stubs/time_rtc.su

.PHONY: clean-Tests-2f-stubs
