################################################################################
# MRS Version: 1.9.1
# 自动生成的文件。不要编辑！
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include Startup/subdir.mk
-include Peripheral/src/subdir.mk
-include Debug/subdir.mk
-include Core/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
USBPD_SRC.hex \

SECONDARY_LIST += \
USBPD_SRC.lst \

SECONDARY_SIZE += \
USBPD_SRC.siz \


# 所有目标
all: USBPD_SRC.elf secondary-outputs

# 工具调用
USBPD_SRC.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imacxw -mabi=ilp32 -msmall-data-limit=8 -msave-restore -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-common -Wunused -Wuninitialized  -g -T "C:\Users\87407\Downloads\CH32X035EVT(1)\EVT\EXAM\SRC\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -Wl,-Map,"USBPD_SRC.map" --specs=nano.specs --specs=nosys.specs -o "USBPD_SRC.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
USBPD_SRC.hex: USBPD_SRC.elf
	@	riscv-none-embed-objcopy -O ihex "USBPD_SRC.elf"  "USBPD_SRC.hex"
	@	@
USBPD_SRC.lst: USBPD_SRC.elf
	@	riscv-none-embed-objdump --all-headers --demangle --disassemble -M xw "USBPD_SRC.elf" > "USBPD_SRC.lst"
	@	@
USBPD_SRC.siz: USBPD_SRC.elf
	@	riscv-none-embed-size --format=berkeley "USBPD_SRC.elf"
	@	@
# 其他目标
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) USBPD_SRC.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
