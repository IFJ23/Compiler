# Include the header file of your code
#include "parser.h"

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic

# Executable name
EXECUTABLE = compiler

# Source files
SRCS = main.c scanner.c parser.c expression.c symtable.c structures.c generator.c error.c

# Object files (auto-generated)
OBJS = $(SRCS:.c=.o)

# Default target
all: $(EXECUTABLE)

# Linking rule
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Compilation rule
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(EXECUTABLE)
