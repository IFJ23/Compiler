# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic

# Executable name
EXECUTABLE = compiler

# Source files
SRCS = main.c ./SRC/scanner.c ./SRC/parser.c ./SRC/expression.c ./SRC/symtable.c ./SRC/structures.c ./SRC/generator.c ./SRC/error.c

# Object files (auto-generated)
OBJS = $(SRCS:.c=.o)

# Linking rule
$(EXECUTABLE): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Compilation rule
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(EXECUTABLE)
