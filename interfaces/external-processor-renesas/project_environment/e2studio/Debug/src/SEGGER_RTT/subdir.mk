################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/SEGGER_RTT/SEGGER_RTT.c \
../src/SEGGER_RTT/SEGGER_RTT_printf.c 

OBJS += \
./src/SEGGER_RTT/SEGGER_RTT.o \
./src/SEGGER_RTT/SEGGER_RTT_printf.o 

C_DEPS += \
./src/SEGGER_RTT/SEGGER_RTT.d \
./src/SEGGER_RTT/SEGGER_RTT_printf.d 


# Each subdirectory must supply rules for building sources it contributes
src/SEGGER_RTT/%.o: ../src/SEGGER_RTT/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m23 -mthumb -O3 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal  -g -D_RENESAS_RA_ -I"D:\Bitbucket\sdk6\interfaces\external-processor-renesas\project_environment\e2studio\src" -I"D:\Bitbucket\sdk6\interfaces\external-processor-renesas\project_environment\e2studio\ra\fsp\inc" -I"D:\Bitbucket\sdk6\interfaces\external-processor-renesas\project_environment\e2studio\ra\fsp\inc\api" -I"D:\Bitbucket\sdk6\interfaces\external-processor-renesas\project_environment\e2studio\ra\fsp\inc\instances" -I"D:\Bitbucket\sdk6\interfaces\external-processor-renesas\project_environment\e2studio\ra\arm\CMSIS_5\CMSIS\Core\Include" -I"D:\Bitbucket\sdk6\interfaces\external-processor-renesas\project_environment\e2studio\ra_gen" -I"D:\Bitbucket\sdk6\interfaces\external-processor-renesas\project_environment\e2studio\ra_cfg\fsp_cfg\bsp" -I"D:\Bitbucket\sdk6\interfaces\external-processor-renesas\project_environment\e2studio\ra_cfg\fsp_cfg" -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<"
	@echo 'Finished building: $<'
	@echo ' '


