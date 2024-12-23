# Compiler and flags
CC = gcc
CFLAGS = -g -Iinclude -Iinclude/core -Iinclude/utils -Iinclude/demo -Iinclude/components
LDFLAGS = -lgdi32 -lcomdlg32 -mwindows

# Directories
SRCDIR = src
BUILDDIR = build

# Output executable
OUT = $(BUILDDIR)/my_program.exe

# Find all source files in the src directory and subdirectories, including main.c
SRCS = $(wildcard $(SRCDIR)/**/*.c) $(wildcard $(SRCDIR)/*.c)

# Object files will go into the build directory
OBJS = $(SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

# Default target
all: $(OUT)

# Link object files into the final executable
$(OUT): $(OBJS)
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build directory
clean:
	@if exist "$(BUILDDIR)" rmdir /s /q "$(BUILDDIR)"

.PHONY: all clean
