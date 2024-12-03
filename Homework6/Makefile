# Makefile for Homework 6

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Libraries to link with (for macOS)
LIBS = -framework OpenGL -framework GLUT

# Executable name
TARGET = hw6

# Source files
SRCS = main.c loadtexbmp.c errcheck.c fatal.c projection.c print.c loadobj.c

# Object files
OBJS = $(SRCS:.c=.o)

# Build target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Rule for creating object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean the directory
clean:
	rm -f $(OBJS) $(TARGET)

# Zip the submission (excluding unwanted files)
zip:
	zip -r hw6_submission.zip . -x "*.DS_Store" -x "*/__MACOSX/*" -x "*.o" -x "$(TARGET)"
