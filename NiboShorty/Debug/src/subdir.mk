################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/NiboShorty.c \
../src/n2sound.c \
../src/uart0.c 

OBJS += \
./src/NiboShorty.o \
./src/n2sound.o \
./src/uart0.o 

C_DEPS += \
./src/NiboShorty.d \
./src/n2sound.d \
./src/uart0.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/Users/philipmitzlaff/Documents/TH_Wildau/5. Semester/Eingebettete Systeme und Robotik 1/nibolib-20110603/include" -D_NIBO_2_ -DNIBO_USE_NDS3 -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


