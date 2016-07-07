CFLAGS = -c -Wall -mmcu=atmega32 -DF_CPU=8000000
EXEC = firmware
OBJ = main.o gpio.o uart_atmega32.o board.o

.PHONY: all clean program

all: $(OBJ) $(EXEC)

$(EXEC): $(OBJ) 
	avr-gcc $(CFLAGS) $(OBJ) -o $@

$(OBJ): config.h uart.h gpio.h board.h types.h

clean:
	rm -rf *.o hello

program: $(EXEC)
	avrdude
