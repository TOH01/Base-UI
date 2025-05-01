# Compiler and flags
CC = gcc
CFLAGS = -g -Iinclude -Iinclude/core -Iinclude/utils -Iinclude/application -Iinclude/components
LDFLAGS = -lgdi32 -lcomdlg32 -mconsole

# Directories
SRCDIR = src
BUILDDIR = build

# Output executable
OUT = $(BUILDDIR)/my_program.exe

# Find all source files in the src directory and subdirectories, including main.c
SRCS = $(shell find $(SRCDIR) -name '*.c')
OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

# Default target
all: $(OUT)

# Link object files into the final executable
$(OUT): $(OBJS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build directory
clean:
	rm -rf $(BUILDDIR)

.PHONY: all clean
