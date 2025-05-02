# Compiler and flags
CC = gcc
CFLAGS = -g -Iinclude -Iinclude/core -Iinclude/utils -Iinclude/application -Iinclude/components
LDFLAGS = -lgdi32 -lcomdlg32 -mconsole

# Directories
SRCDIR = src
BUILDDIR = build

# Output executable
OUT = $(BUILDDIR)/my_program.exe

# Find source files manually (Windows-compatible)
SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*/*.c) $(wildcard $(SRCDIR)/*/*/*.c)
OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

# Default target
all: $(OUT)

# Link object files
$(OUT): $(OBJS)
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build directory
clean:
	@if exist "$(BUILDDIR)" rmdir /s /q "$(BUILDDIR)"

.PHONY: all clean
