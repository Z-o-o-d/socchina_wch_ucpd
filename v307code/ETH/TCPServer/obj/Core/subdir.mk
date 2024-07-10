################################################################################
# MRS Version: 1.9.1
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/87407/OneDrive/EMBcompCRPS_CH32/relese/SRC/Core/core_riscv.c 

OBJS += \
./Core/core_riscv.o 

C_DEPS += \
./Core/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
Core/core_riscv.o: C:/Users/87407/OneDrive/EMBcompCRPS_CH32/relese/SRC/Core/core_riscv.c
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -I"C:\Users\87407\OneDrive\EMBcompCRPS_CH32\relese\ETH\NetLib" -I"C:\Users\87407\OneDrive\EMBcompCRPS_CH32\relese\SRC\Core" -I"C:\Users\87407\OneDrive\EMBcompCRPS_CH32\relese\SRC\Debug" -I"C:\Users\87407\OneDrive\EMBcompCRPS_CH32\relese\SRC\Peripheral\inc" -I"C:\Users\87407\OneDrive\EMBcompCRPS_CH32\relese\ETH\TCPServer\User" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

