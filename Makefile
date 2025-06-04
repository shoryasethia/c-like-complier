# Makefile for C-like Compiler

# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g # -Wall, -Wextra for warnings, -g for debug info
LDFLAGS =

# Directories
SRCDIR = src
BUILDDIR = build
TESTDIR = tests

# Source files for the main compiler executable
SRCS = $(SRCDIR)/main.cpp \
       $(SRCDIR)/Lexer.cpp \
       $(SRCDIR)/Token.cpp \
       # Add other .cpp files here as you create them (e.g., parser.cpp)

# Object files (compiled .cpp files)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(BUILDDIR)/%.o, $(SRCS))

# Main executable name
EXECUTABLE = $(BUILDDIR)/c-like-compiler

# Test files for the lexer
TEST_SRCS = $(TESTDIR)/lexer_tests.cpp

# Test object files: all test sources + all main source objects *except* main.o
TEST_OBJS = $(patsubst $(TESTDIR)/%.cpp, $(BUILDDIR)/%.test.o, $(TEST_SRCS)) \
            $(filter-out $(BUILDDIR)/main.o, $(OBJS))

# Test executable name
TEST_EXECUTABLE = $(BUILDDIR)/run_tests

# Phony targets: actions that don't correspond to file names
.PHONY: all test clean

# Default target: builds the main executable
all: $(EXECUTABLE)

# Rule to build the main executable
$(EXECUTABLE): $(OBJS)
	@mkdir -p $(@D) # Create the build directory if it doesn't exist
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Built $(EXECUTABLE)"

# Generic rule to compile any .cpp file from SRCDIR to a .o file in BUILDDIR
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -c $< -o $@
	@echo "Compiled $<"

# --- Test Targets ---

# Test runner target
test: $(TEST_EXECUTABLE)
	@echo "Running tests..."
	./$(TEST_EXECUTABLE)
	@echo "Tests finished."

# Rule to build the test executable
$(TEST_EXECUTABLE): $(TEST_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(TEST_OBJS) -o $@ $(LDFLAGS)
	@echo "Built $(TEST_EXECUTABLE)"

# Generic rule to compile any .cpp file from TESTDIR to a .test.o file in BUILDDIR
$(BUILDDIR)/%.test.o: $(TESTDIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(SRCDIR) -I$(TESTDIR) -c $< -o $@
	@echo "Compiled $<"

# --- Clean Target ---
clean:
	@echo "Cleaning build files..."
	rm -f $(BUILDDIR)/*.o
	rm -f $(EXECUTABLE) $(TEST_EXECUTABLE)
	@rmdir $(BUILDDIR) 2>/dev/null || true # Remove build dir if empty, suppress error if not
	@echo "Cleaned."