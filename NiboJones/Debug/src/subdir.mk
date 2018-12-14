################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/hendriktanke/Desktop/git/IndiaNiboJones/NiboJones/gfxOutput.c \
/Users/hendriktanke/Desktop/git/IndiaNiboJones/NiboJones/main.c \
/Users/hendriktanke/Desktop/git/IndiaNiboJones/NiboJones/n2switchS3.c \
/Users/hendriktanke/Desktop/git/IndiaNiboJones/NiboJones/protocols.c \
/Users/hendriktanke/Desktop/git/IndiaNiboJones/NiboJones/uart0.c 

OBJS += \
./src/gfxOutput.o \
./src/main.o \
./src/n2switchS3.o \
./src/protocols.o \
./src/uart0.o 

C_DEPS += \
./src/gfxOutput.d \
./src/main.d \
./src/n2switchS3.d \
./src/protocols.d \
./src/uart0.d 


# Each subdirectory must supply rules for building sources it contributes
src/gfxOutput.o: /Users/hendriktanke/Desktop/git/IndiaNiboJones/NiboJones/gfxOutput.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/Users/hendriktanke/Desktop/Ablage/Uni/ESR/nibolib-20110603/include -D_NIBO_2_ -DNIBO_USE_NDS3 -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -D_NIBO_2_ -mmcu=atmega128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/main.o: /Users/hendriktanke/Desktop/git/IndiaNiboJones/NiboJones/main.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/Users/hendriktanke/Desktop/Ablage/Uni/ESR/nibolib-20110603/include -D_NIBO_2_ -DNIBO_USE_NDS3 -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -D_NIBO_2_ -mmcu=atmega128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/n2switchS3.o: /Users/hendriktanke/Desktop/git/IndiaNiboJones/NiboJones/n2switchS3.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/Users/hendriktanke/Desktop/Ablage/Uni/ESR/nibolib-20110603/include -D_NIBO_2_ -DNIBO_USE_NDS3 -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -D_NIBO_2_ -mmcu=atmega128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/protocols.o: /Users/hendriktanke/Desktop/git/IndiaNiboJones/NiboJones/protocols.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/Users/hendriktanke/Desktop/Ablage/Uni/ESR/nibolib-20110603/include -D_NIBO_2_ -DNIBO_USE_NDS3 -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -D_NIBO_2_ -mmcu=atmega128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/uart0.o: /Users/hendriktanke/Desktop/git/IndiaNiboJones/NiboJones/uart0.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/Users/hendriktanke/Desktop/Ablage/Uni/ESR/nibolib-20110603/include -D_NIBO_2_ -DNIBO_USE_NDS3 -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -D_NIBO_2_ -mmcu=atmega128 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


