CC = avr-gcc
EXEC = firmware.hex
CFLAGS = -Wall -DF_CPU=8000000 -I/usr/lib/avr/include \
 -mmcu=atmega32 -Os -fpack-struct -fshort-enums -std=gnu99 \
 -funsigned-char -funsigned-bitfields 

OBJ = board.o gpio.o gpio_keys.o gpio_debouncer.o \
 idle.o leds.o list.o sys_timer.o uart_atmega32.o \
 train_controller.o ac_motors.c

.PHONY: all clean program fuse

all: $(EXEC)

$(EXEC): $(OBJ) 
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

$(OBJ): config.h types.h gpio.h gpio_debouncer.h \
 gpio_keys.h uart.h list.h sys_timer.h idle.h board.h \
 types.h train_controller.h ac_motors.h

clean:
	rm -rf *.o $(EXEC)

program: $(EXEC)
	sudo avrdude -p m32 -c usbasp -e -U flash:w:$(EXEC)

fuse:
	sudo avrdude -p m32 -c usbasp -U lfuse:w:0x8e:m -U hfuse:w:0xd9:m
