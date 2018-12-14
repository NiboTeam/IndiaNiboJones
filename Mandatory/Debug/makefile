################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
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
LSS += \
Mandatory.lss \

FLASH_IMAGE += \
Mandatory.hex \

SIZEDUMMY += \
sizedummy \


# All Target
all: Mandatory.elf secondary-outputs

# Tool invocations
Mandatory.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Mandatory.map -Wl,-u,vfprintf -L"/Users/philipmitzlaff/Documents/TH_Wildau/5. Semester/Eingebettete Systeme und Robotik 1/nibolib-20110603/lib" -mmcu=atmega128 -o "Mandatory.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Mandatory.lss: Mandatory.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Mandatory.elf  >"Mandatory.lss"
	@echo 'Finished building: $@'
	@echo ' '

Mandatory.hex: Mandatory.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Mandatory.elf  "Mandatory.hex"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Mandatory.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega128 Mandatory.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) Mandatory.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents

-include ../makefile.targets
