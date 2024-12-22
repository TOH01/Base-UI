# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -g -I./include 

LDFLAGS = -lgdi32 -lcomdlg32

# Define the source and object files using wildcard to include all .c files in the src directory
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)  # Place object files in build/ directory

# Define the output executable
EXEC = build/my_program.exe

# Default target (this will be the first target in the Makefile)
all: $(EXEC)

# Rule to link the object files and create the executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Rule to compile each .c file into .o file and store them in build/ directory
build/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up object files and the executable
clean:
	rm -f build/*.o $(EXEC)

# Rebuild everything
rebuild: clean all
