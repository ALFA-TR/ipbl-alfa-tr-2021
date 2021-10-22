################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../State\ Machine/sm_adp-flual.c \
../State\ Machine/sm_transition-tests.c 

OBJS += \
./State\ Machine/sm_adp-flual.o \
./State\ Machine/sm_transition-tests.o 

C_DEPS += \
./State\ Machine/sm_adp-flual.d \
./State\ Machine/sm_transition-tests.d 


# Each subdirectory must supply rules for building sources it contributes
State\ Machine/sm_adp-flual.o: ../State\ Machine/sm_adp-flual.c State\ Machine/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"N:/Transition_Test-ADP-FLUAL/State Machine" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"State Machine/sm_adp-flual.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
State\ Machine/sm_transition-tests.o: ../State\ Machine/sm_transition-tests.c State\ Machine/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L432xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I"N:/Transition_Test-ADP-FLUAL/State Machine" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"State Machine/sm_transition-tests.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

