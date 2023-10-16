# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic

# Executable name
EXECUTABLE = my_tests

# Source files
SRCS = test.c ../SRC/scanner.c

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
