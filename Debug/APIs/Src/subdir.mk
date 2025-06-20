################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../APIs/Src/DHT22.c \
../APIs/Src/DHT22_Hardware.c \
../APIs/Src/DWT_Delay.c \
../APIs/Src/ParticulateDataAnalyzer.c \
../APIs/Src/data_logger.c \
../APIs/Src/fatfs_sd.c \
../APIs/Src/microSD.c \
../APIs/Src/microSD_utils.c \
../APIs/Src/mp_sensors_info.c \
../APIs/Src/pm25_avg10.c \
../APIs/Src/proceso_observador.c \
../APIs/Src/rtc_buildtime.c \
../APIs/Src/rtc_ds1307_for_stm32_hal.c \
../APIs/Src/rtc_ds3231_for_stm32_hal.c \
../APIs/Src/sensor.c \
../APIs/Src/shdlc.c \
../APIs/Src/sistema_init.c \
../APIs/Src/sps30_comm.c \
../APIs/Src/sps30_multi.c \
../APIs/Src/time_rtc.c \
../APIs/Src/uart.c

OBJS += \
./APIs/Src/DHT22.o \
./APIs/Src/DHT22_Hardware.o \
./APIs/Src/DWT_Delay.o \
./APIs/Src/ParticulateDataAnalyzer.o \
./APIs/Src/data_logger.o \
./APIs/Src/fatfs_sd.o \
./APIs/Src/microSD.o \
./APIs/Src/microSD_utils.o \
./APIs/Src/mp_sensors_info.o \
./APIs/Src/pm25_avg10.o \
./APIs/Src/proceso_observador.o \
./APIs/Src/rtc_buildtime.o \
./APIs/Src/rtc_ds1307_for_stm32_hal.o \
./APIs/Src/rtc_ds3231_for_stm32_hal.o \
./APIs/Src/sensor.o \
./APIs/Src/shdlc.o \
./APIs/Src/sistema_init.o \
./APIs/Src/sps30_comm.o \
./APIs/Src/sps30_multi.o \
./APIs/Src/time_rtc.o \
./APIs/Src/uart.o

C_DEPS += \
./APIs/Src/DHT22.d \
./APIs/Src/DHT22_Hardware.d \
./APIs/Src/DWT_Delay.d \
./APIs/Src/ParticulateDataAnalyzer.d \
./APIs/Src/data_logger.d \
./APIs/Src/fatfs_sd.d \
./APIs/Src/microSD.d \
./APIs/Src/microSD_utils.d \
./APIs/Src/mp_sensors_info.d \
./APIs/Src/pm25_avg10.d \
./APIs/Src/proceso_observador.d \
./APIs/Src/rtc_buildtime.d \
./APIs/Src/rtc_ds1307_for_stm32_hal.d \
./APIs/Src/rtc_ds3231_for_stm32_hal.d \
./APIs/Src/sensor.d \
./APIs/Src/shdlc.d \
./APIs/Src/sistema_init.d \
./APIs/Src/sps30_comm.d \
./APIs/Src/sps30_multi.d \
./APIs/Src/time_rtc.d \
./APIs/Src/uart.d


# Each subdirectory must supply rules for building sources it contributes
APIs/Src/%.o APIs/Src/%.su APIs/Src/%.cyclo: ../APIs/Src/%.c APIs/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F429xx -c -I../Core/Inc -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/STM32F4xx_HAL_Driver/Inc -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/CMSIS/Device/ST/STM32F4xx/Include -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Drivers/CMSIS/Include -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs" -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs/Inc" -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs/Src" -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/APIs/Config" -I../FATFS/Target -I../FATFS/App -I/home/lgomez/STM32Cube/Repository/STM32Cube_FW_F4_V1.27.1/Middlewares/Third_Party/FatFs/src -I"/home/lgomez/Documentos/MAGISTER_UBA/TESIS/Tesis_STM32/Tesis_SPS30/Tests" -O0 -ffunction-sections -fdata-sections -Wall -u _printf_float -u _scanf_float -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-APIs-2f-Src

clean-APIs-2f-Src:
	-$(RM) ./APIs/Src/DHT22.cyclo ./APIs/Src/DHT22.d ./APIs/Src/DHT22.o ./APIs/Src/DHT22.su ./APIs/Src/DHT22_Hardware.cyclo ./APIs/Src/DHT22_Hardware.d ./APIs/Src/DHT22_Hardware.o ./APIs/Src/DHT22_Hardware.su ./APIs/Src/DWT_Delay.cyclo ./APIs/Src/DWT_Delay.d ./APIs/Src/DWT_Delay.o ./APIs/Src/DWT_Delay.su ./APIs/Src/ParticulateDataAnalyzer.cyclo ./APIs/Src/ParticulateDataAnalyzer.d ./APIs/Src/ParticulateDataAnalyzer.o ./APIs/Src/ParticulateDataAnalyzer.su ./APIs/Src/data_logger.cyclo ./APIs/Src/data_logger.d ./APIs/Src/data_logger.o ./APIs/Src/data_logger.su ./APIs/Src/fatfs_sd.cyclo ./APIs/Src/fatfs_sd.d ./APIs/Src/fatfs_sd.o ./APIs/Src/fatfs_sd.su ./APIs/Src/microSD.cyclo ./APIs/Src/microSD.d ./APIs/Src/microSD.o ./APIs/Src/microSD.su ./APIs/Src/microSD_utils.cyclo ./APIs/Src/microSD_utils.d ./APIs/Src/microSD_utils.o ./APIs/Src/microSD_utils.su ./APIs/Src/mp_sensors_info.cyclo ./APIs/Src/mp_sensors_info.d ./APIs/Src/mp_sensors_info.o ./APIs/Src/mp_sensors_info.su ./APIs/Src/pm25_avg10.cyclo ./APIs/Src/pm25_avg10.d ./APIs/Src/pm25_avg10.o ./APIs/Src/pm25_avg10.su ./APIs/Src/proceso_observador.cyclo ./APIs/Src/proceso_observador.d ./APIs/Src/proceso_observador.o ./APIs/Src/proceso_observador.su ./APIs/Src/rtc_buildtime.cyclo ./APIs/Src/rtc_buildtime.d ./APIs/Src/rtc_buildtime.o ./APIs/Src/rtc_buildtime.su ./APIs/Src/rtc_ds1307_for_stm32_hal.cyclo ./APIs/Src/rtc_ds1307_for_stm32_hal.d ./APIs/Src/rtc_ds1307_for_stm32_hal.o ./APIs/Src/rtc_ds1307_for_stm32_hal.su ./APIs/Src/rtc_ds3231_for_stm32_hal.cyclo ./APIs/Src/rtc_ds3231_for_stm32_hal.d ./APIs/Src/rtc_ds3231_for_stm32_hal.o ./APIs/Src/rtc_ds3231_for_stm32_hal.su ./APIs/Src/sensor.cyclo ./APIs/Src/sensor.d ./APIs/Src/sensor.o ./APIs/Src/sensor.su ./APIs/Src/shdlc.cyclo ./APIs/Src/shdlc.d ./APIs/Src/shdlc.o ./APIs/Src/shdlc.su ./APIs/Src/sistema_init.cyclo ./APIs/Src/sistema_init.d ./APIs/Src/sistema_init.o ./APIs/Src/sistema_init.su ./APIs/Src/sps30_comm.cyclo ./APIs/Src/sps30_comm.d ./APIs/Src/sps30_comm.o ./APIs/Src/sps30_comm.su ./APIs/Src/sps30_multi.cyclo ./APIs/Src/sps30_multi.d ./APIs/Src/sps30_multi.o ./APIs/Src/sps30_multi.su ./APIs/Src/time_rtc.cyclo ./APIs/Src/time_rtc.d ./APIs/Src/time_rtc.o ./APIs/Src/time_rtc.su ./APIs/Src/uart.cyclo ./APIs/Src/uart.d ./APIs/Src/uart.o ./APIs/Src/uart.su

.PHONY: clean-APIs-2f-Src
