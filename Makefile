CC = gcc
CFLAGS = -Wall -Wextra -O2
SRC = src/sudoku.c
TARGET = sudoku

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET)

.PHONY: all clean