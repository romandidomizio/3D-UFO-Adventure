# Makefile for Homework 5

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Libraries to link with
LIBS = -framework OpenGL -framework GLUT

# Executable name
TARGET = hw5

# Source files
SRCS = main.c

# Object files
OBJS = $(SRCS:.c=.o)

# Build target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Clean the directory
clean:
	rm -f $(OBJS) $(TARGET)

# Zip the files
zip:
	zip -r ../hw5_submission.zip . -x "*.DS_Store" -x "*/__MACOSX/*"
