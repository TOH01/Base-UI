# for release make BUILD_TYPE=release
# Compiler
CC = gcc
WINDRES = windres

# Directories
SRCDIR = src
TESTDIR = tests
BUILDDIR = build

# Output executable names
OUT = $(BUILDDIR)/my_program.exe
TESTOUT = $(BUILDDIR)/test_program.exe

# Resource files
RCFILE = $(SRCDIR)/application/resource.rc
RESFILE = $(BUILDDIR)/resource.res

# Source files
SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*/*.c) $(wildcard $(SRCDIR)/*/*/*.c)
TEST_SRCS = $(wildcard $(TESTDIR)/*.c) $(wildcard $(TESTDIR)/*/*.c) $(wildcard $(TESTDIR)/*/*/*.c)

# Object files
OBJS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))
TEST_OBJS = $(patsubst $(TESTDIR)/%.c, $(BUILDDIR)/%.o, $(TEST_SRCS))

# Build type: debug or release (default: debug)
BUILD_TYPE ?= debug

# Flags per build type
ifeq ($(BUILD_TYPE),release)
    CFLAGS = -O3 -march=native -flto -ffast-math -DNDEBUG \
             -Iinclude -Iinclude/core -Iinclude/utils -Iinclude/application -Iinclude/components -D_WIN32_WINNT=0x0A00
    LDFLAGS = -lgdi32 -lcomdlg32 -mwindows -luser32 -luxtheme
else
    # debug
    CFLAGS = -g -O0 -DDEBUG \
             -Iinclude -Iinclude/core -Iinclude/utils -Iinclude/application -Iinclude/components \
             -Wall -Wextra -Werror -Wstrict-prototypes -D_WIN32_WINNT=0x0A00
    LDFLAGS = -lgdi32 -lcomdlg32 -mconsole -luser32 -luxtheme
endif

# Default target
all: $(OUT)

# Build main program with resource file
$(OUT): $(OBJS) $(RESFILE)
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build test program (no resource file)
$(TESTOUT): $(OBJS) $(TEST_OBJS)
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile main source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Compile test source files
$(BUILDDIR)/%.o: $(TESTDIR)/%.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Compile the .rc file to .res
$(RESFILE): $(RCFILE)
	@if not exist "$(BUILDDIR)" mkdir "$(BUILDDIR)"
	$(WINDRES) $< -O coff -o $@

# Clean build directory
clean:
	@if exist "$(BUILDDIR)" rmdir /s /q "$(BUILDDIR)"

# Run all tests
test: $(TESTOUT)
	@echo "Running all tests..."
	./$(TESTOUT)

.PHONY: all clean test
