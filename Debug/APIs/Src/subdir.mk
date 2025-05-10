################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APIs/Src/ParticulateDataAnalyzer.c \
../APIs/Src/data_logger.c \
../APIs/Src/proceso_oservador.c \
../APIs/Src/rtc_ds1307_for_stm32_hal.c \
../APIs/Src/shdlc.c \
../APIs/Src/sps30_comm.c \
../APIs/Src/sps30_multi.c \
../APIs/Src/time_rtc.c \
../APIs/Src/uart_printing.c 

OBJS += \
./APIs/Src/ParticulateDataAnalyzer.o \
./APIs/Src/data_logger.o \
./APIs/Src/proceso_oservador.o \
./APIs/Src/rtc_ds1307_for_stm32_hal.o \
./APIs/Src/shdlc.o \
./APIs/Src/sps30_comm.o \
./APIs/Src/sps30_multi.o \
./APIs/Src/time_rtc.o \
./APIs/Src/uart_printing.o 

C_DEPS += \
./APIs/Src/ParticulateDataAnalyzer.d \
./APIs/Src/data_logger.d \
./APIs/Src/proceso_oservador.d \
./APIs/Src/rtc_ds1307_for_stm32_hal.d \
./APIs/Src/shdlc.d \
./APIs/Src/sps30_comm.d \
./APIs/Src/sps30_multi.d \
./APIs/Src/time_rtc.d \
./APIs/Src/uart_printing.d 


# Each subdirectory must supply rules for building sources it contributes
APIs/Src/%.o APIs/Src/%.su APIs/Src/%.cyclo: ../APIs/Src/%.c APIs/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/STM32F4xx_HAL_Driver/Inc -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/CMSIS/Device/ST/STM32F4xx/Include -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/CMSIS/Include -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs" -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs/Inc" -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs/Src" -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -u _scanf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-APIs-2f-Src

clean-APIs-2f-Src:
	-$(RM) ./APIs/Src/ParticulateDataAnalyzer.cyclo ./APIs/Src/ParticulateDataAnalyzer.d ./APIs/Src/ParticulateDataAnalyzer.o ./APIs/Src/ParticulateDataAnalyzer.su ./APIs/Src/data_logger.cyclo ./APIs/Src/data_logger.d ./APIs/Src/data_logger.o ./APIs/Src/data_logger.su ./APIs/Src/proceso_oservador.cyclo ./APIs/Src/proceso_oservador.d ./APIs/Src/proceso_oservador.o ./APIs/Src/proceso_oservador.su ./APIs/Src/rtc_ds1307_for_stm32_hal.cyclo ./APIs/Src/rtc_ds1307_for_stm32_hal.d ./APIs/Src/rtc_ds1307_for_stm32_hal.o ./APIs/Src/rtc_ds1307_for_stm32_hal.su ./APIs/Src/shdlc.cyclo ./APIs/Src/shdlc.d ./APIs/Src/shdlc.o ./APIs/Src/shdlc.su ./APIs/Src/sps30_comm.cyclo ./APIs/Src/sps30_comm.d ./APIs/Src/sps30_comm.o ./APIs/Src/sps30_comm.su ./APIs/Src/sps30_multi.cyclo ./APIs/Src/sps30_multi.d ./APIs/Src/sps30_multi.o ./APIs/Src/sps30_multi.su ./APIs/Src/time_rtc.cyclo ./APIs/Src/time_rtc.d ./APIs/Src/time_rtc.o ./APIs/Src/time_rtc.su ./APIs/Src/uart_printing.cyclo ./APIs/Src/uart_printing.d ./APIs/Src/uart_printing.o ./APIs/Src/uart_printing.su

.PHONY: clean-APIs-2f-Src

