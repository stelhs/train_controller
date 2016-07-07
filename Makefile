CC = avr-gcc
CFLAGS = -Wall -DF_CPU=8000000 -I/usr/lib/avr/include \
 -mmcu=atmega32 -Os -fpack-struct -fshort-enums -std=gnu99 \
 -funsigned-char -funsigned-bitfields 

EXEC = firmware
OBJ = board.o gpio.o gpio_keys.o gpio_debouncer.o \
 idle.o leds.o list.o sys_timer.o uart_atmega32.o

.PHONY: all clean program

all: $(EXEC)

$(EXEC): $(OBJ) 
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

$(OBJ): config.h types.h gpio.h gpio_debouncer.h \
 gpio_keys.h uart.h list.h sys_timer.h idle.h board.h types.h

clean:
	rm -rf *.o $(EXEC)

program: $(EXEC)
	avrdude
