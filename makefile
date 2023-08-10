CC = gcc
CFLAGS = -Wall -Wextra --std=gnu99
SRC = bitmap.c buddy_allocator.c pseudo_malloc.c main.c
OBJ = $(SRC:.c=.o)
TARGET = so_example

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)
