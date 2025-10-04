# for release make BUILD_TYPE=release
# Compiler
CC = gcc
WINDRES = windres

# Directories
SRCDIR = src
TESTDIR = tests
BUILDDIR = build
DEMOSDIR = $(SRCDIR)/application

# Choose demo: calendar or narcia
DEMO ?= calendar
DEMODIR = $(DEMOSDIR)/$(DEMO)

# Output executable names
OUT = $(BUILDDIR)/my_program.exe
TESTOUT = $(BUILDDIR)/test_program.exe

# Resource files
RCFILE = $(DEMODIR)/resource.rc
RESFILE = $(BUILDDIR)/$(DEMO)_resource.res

# Framework source files
FRAMEWORK_SRCS = $(wildcard $(SRCDIR)/*.c) $(wildcard $(SRCDIR)/*/*.c)
# Demo source files (only the selected demo)
DEMO_SRCS = $(wildcard $(DEMODIR)/*.c) $(wildcard $(DEMODIR)/*/*.c)
# Combined
SRCS = $(FRAMEWORK_SRCS) $(DEMO_SRCS)

# Object files
OBJS = $(patsubst $(SRCDIR)/%.c,$(BUILDDIR)/%.o,$(FRAMEWORK_SRCS)) \
       $(patsubst $(DEMODIR)/%.c,$(BUILDDIR)/%.o,$(DEMO_SRCS))

# Test sources and objects
TEST_SRCS = $(wildcard $(TESTDIR)/*.c) $(wildcard $(TESTDIR)/*/*.c)
TEST_OBJS = $(patsubst $(TESTDIR)/%.c,$(BUILDDIR)/%.o,$(TEST_SRCS))

# Build type: debug or release (default: debug)
BUILD_TYPE ?= debug

# Flags per build type
ifeq ($(BUILD_TYPE),release)
    CFLAGS = -O3 -march=native -flto -ffast-math -DNDEBUG \
             -Iinclude -Iinclude/core -Iinclude/utils -Iinclude/application -Iinclude/components -D_WIN32_WINNT=0x0A00
    LDFLAGS = -lgdi32 -lcomdlg32 -mwindows -luser32 -luxtheme -lMsimg32
else
    # debug
    CFLAGS = -g -O0 -DDEBUG \
             -Iinclude -Iinclude/core -Iinclude/utils -Iinclude/application -Iinclude/components \
             -Wall -Wextra -Werror -Wstrict-prototypes -D_WIN32_WINNT=0x0A00
    LDFLAGS = -lgdi32 -lcomdlg32 -mwindows -luser32 -luxtheme -lMsimg32
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

# Compile framework source files
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) $(CFLAGS) -c $< -o $@

# Compile demo source files (only selected demo)
$(BUILDDIR)/%.o: $(DEMODIR)/%.c
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
