################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../ProcessMaps.cc \
../elf_parser.cc \
../injector.cc \
../ptrace.cc \
../utils.cc 

OBJS += \
./ProcessMaps.o \
./elf_parser.o \
./injector.o \
./ptrace.o \
./utils.o 

CC_DEPS += \
./ProcessMaps.d \
./elf_parser.d \
./injector.d \
./ptrace.d \
./utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -DDEBUG -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


