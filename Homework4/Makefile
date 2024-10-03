# Makefile for Homework 4

# Compiler and flags
CC = g++
CFLAGS = -Wall -g
LIBS = -framework OpenGL -framework GLUT

# Target executable
TARGET = hw4

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f $(OBJS) $(TARGET)
