CFLAGS = -c -Wall -mmcu=atmega32 -DF_CPU=8000000
EXEC = firmware
OBJ = board.o gpio.o gpio_keys.o gpio_debouncer.o \
 idle.o leds.o list.o sys_timer.o uart_atmega32.o

.PHONY: all clean program

all: $(OBJ) $(EXEC)

$(EXEC): $(OBJ) 
	avr-gcc $(CFLAGS) $(OBJ) -o $@

$(OBJ): config.h types.h gpio.h gpio_debouncer.h \
 gpio_keys.h uart.h list.h sys_timer.h idle.h board.h types.h

clean:
	rm -rf *.o $(EXEC)

program: $(EXEC)
	avrdude
