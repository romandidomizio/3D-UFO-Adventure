# Makefile for Homework 3 - Roman Di Domizio

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g -std=c99
LIBS = -framework OpenGL -framework GLUT

# Target executable
TARGET = hw3

# Source files
SRCS = main.c

# Build the project
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Clean up object files and executable
clean:
	rm -f $(TARGET)
