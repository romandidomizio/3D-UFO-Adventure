# Makefile for UFO Pilot

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Detect platform and set libraries accordingly
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S), Linux)
    LIBS = -lGL -lGLU -lglut
else ifeq ($(UNAME_S), Darwin)  # macOS
    LIBS = -framework OpenGL -framework GLUT
else
    $(error "Unsupported platform: $(UNAME_S)")
endif

# Executable name
TARGET = ufo

# Source files
SRCS = main.c loadtexbmp.c errcheck.c fatal.c projection.c print.c loadobj.c

# Object files
OBJS = $(SRCS:.c=.o)

# Default build target
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

# Rule for creating object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean the build directory
clean:
	rm -f $(OBJS) $(TARGET)

# Zip the project (exclude unwanted files)
zip:
	zip -r UFO_Adventure.zip . -x "*.DS_Store" -x "__MACOSX/*" -x "*.o" -x "$(TARGET)"
