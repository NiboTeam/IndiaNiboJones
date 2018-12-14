################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/philipmitzlaff/Documents/TH_Wildau/5.\ Semester/Eingebettete\ Systeme\ und\ Robotik\ 1/IndiaNiboJones/Mandatory/batterystatus.c \
/Users/philipmitzlaff/Documents/TH_Wildau/5.\ Semester/Eingebettete\ Systeme\ und\ Robotik\ 1/IndiaNiboJones/Mandatory/mandatory3.c \
/Users/philipmitzlaff/Documents/TH_Wildau/5.\ Semester/Eingebettete\ Systeme\ und\ Robotik\ 1/IndiaNiboJones/Mandatory/n2switchS3.c \
/Users/philipmitzlaff/Documents/TH_Wildau/5.\ Semester/Eingebettete\ Systeme\ und\ Robotik\ 1/IndiaNiboJones/Mandatory/showdistance.c 

OBJS += \
./src/batterystatus.o \
./src/mandatory3.o \
./src/n2switchS3.o \
./src/showdistance.o 

C_DEPS += \
./src/batterystatus.d \
./src/mandatory.d \
./src/n2switchS3.d \
./src/showdistance.d 


# Each subdirectory must supply rules for building sources it contributes
src/batterystatus.o: /Users/philipmitzlaff/Documents/TH_Wildau/5.\ Semester/Eingebettete\ Systeme\ und\ Robotik\ 1/IndiaNiboJones/Mandatory/batterystatus.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/Users/philipmitzlaff/Documents/TH_Wildau/5. Semester/Eingebettete Systeme und Robotik 1/nibolib-20110603/include" -D_NIBO_2_ -DNIBO_USE_NDS3 -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega128 -DF_CPU=16000000UL -MMD -MP -MF"src/batterystatus.d" -MT"src/batterystatus.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/mandatory3.o: /Users/philipmitzlaff/Documents/TH_Wildau/5.\ Semester/Eingebettete\ Systeme\ und\ Robotik\ 1/IndiaNiboJones/Mandatory/mandatory3.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/Users/philipmitzlaff/Documents/TH_Wildau/5. Semester/Eingebettete Systeme und Robotik 1/nibolib-20110603/include" -D_NIBO_2_ -DNIBO_USE_NDS3 -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega128 -DF_CPU=16000000UL -MMD -MP -MF"src/mandatory.d" -MT"src/mandatory.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/n2switchS3.o: /Users/philipmitzlaff/Documents/TH_Wildau/5.\ Semester/Eingebettete\ Systeme\ und\ Robotik\ 1/IndiaNiboJones/Mandatory/n2switchS3.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/Users/philipmitzlaff/Documents/TH_Wildau/5. Semester/Eingebettete Systeme und Robotik 1/nibolib-20110603/include" -D_NIBO_2_ -DNIBO_USE_NDS3 -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega128 -DF_CPU=16000000UL -MMD -MP -MF"src/n2switchS3.d" -MT"src/n2switchS3.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/showdistance.o: /Users/philipmitzlaff/Documents/TH_Wildau/5.\ Semester/Eingebettete\ Systeme\ und\ Robotik\ 1/IndiaNiboJones/Mandatory/showdistance.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I"/Users/philipmitzlaff/Documents/TH_Wildau/5. Semester/Eingebettete Systeme und Robotik 1/nibolib-20110603/include" -D_NIBO_2_ -DNIBO_USE_NDS3 -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega128 -DF_CPU=16000000UL -MMD -MP -MF"src/showdistance.d" -MT"src/showdistance.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


