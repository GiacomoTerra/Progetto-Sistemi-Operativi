CC = gcc
CFLAGS = -Wall -Wextra -std=c11
SRC = main.c
OBJ = $(SRC:.c=.o)
TARGET = so_example

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
