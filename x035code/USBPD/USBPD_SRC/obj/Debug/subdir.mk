################################################################################
# MRS Version: 1.9.1
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/87407/Downloads/CH32X035EVT(1)/EVT/EXAM/SRC/Debug/debug.c 

OBJS += \
./Debug/debug.o 

C_DEPS += \
./Debug/debug.d 


# Each subdirectory must supply rules for building sources it contributes
Debug/debug.o: C:/Users/87407/Downloads/CH32X035EVT(1)/EVT/EXAM/SRC/Debug/debug.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\87407\Downloads\CH32X035EVT(1)\EVT\EXAM\SRC\Debug" -I"C:\Users\87407\Downloads\CH32X035EVT(1)\EVT\EXAM\SRC\Core" -I"C:\Users\87407\Downloads\CH32X035EVT(1)\EVT\EXAM\USBPD\USBPD_SRC\User" -I"C:\Users\87407\Downloads\CH32X035EVT(1)\EVT\EXAM\SRC\Peripheral\inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

