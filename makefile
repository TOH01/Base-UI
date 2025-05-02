# Compiler and flags
CC = gcc
CFLAGS = -g -Iinclude -Iinclude/core -Iinclude/utils -Iinclude/application -Iinclude/components
LDFLAGS = -lgdi32 -lcomdlg32 -mconsole

# Directories
SRCDIR = src
TESTDIR = tests
BUILDDIR = build

# Output executable for the main program and tests
OUT = $(BUILDDIR)/my_program.exe
TESTOUT = $(BUILDDIR)/test_program.exe

# Find source files manually (Windows-compatible)
SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*/*.c) $(wildcard $(SRCDIR)/*/*/*.c)
TEST_SRCS = $(wildcard $(TESTDIR)/*.c) $(wildcard $(TESTDIR)/*/*.c) $(wildcard $(TESTDIR)/*/*/*.c)

OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))
TEST_OBJS = $(patsubst $(TESTDIR)/%.c, $(BUILDDIR)/%.o, $(TEST_SRCS))

# Default target
all: $(OUT)

# Build the main program
$(OUT): $(OBJS)
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build the test program
$(TESTOUT): $(OBJS) $(TEST_OBJS)
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test files
$(BUILDDIR)/%.o: $(TESTDIR)/%.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build directory
clean:
	@if exist "$(BUILDDIR)" rmdir /s /q "$(BUILDDIR)"

# Run all tests
test: $(TESTOUT)
	@echo "Running all tests..."
	./$(TESTOUT)

.PHONY: all clean test
