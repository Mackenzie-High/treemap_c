# Compiler
CC = gcc
CCFLAGS = -m64 -Wall -Wextra -O2 -I include -g -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function

# Linker
CXX = g++
CXXFLAGS =
LDFLAGS =

# Directories
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build

# Source files and output executables
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
TEST_FILES = $(wildcard $(TEST_DIR)/*.c)
OBJ_FILES = $(filter-out $(BUILD_DIR)/main., $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o))
OBJ_TEST_FILES = $(TEST_FILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%.o)
EXECUTABLE = $(BUILD_DIR)/main
EXECUTABLE_TEST = $(BUILD_DIR)/main

# Default target
all: compile

# Rule to link the main executable
$(EXECUTABLE): $(OBJ_FILES)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Rule to link the test executable
$(EXECUTABLE_TEST): $(OBJ_FILES) $(OBJ_TEST_FILES)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Rule to compile source files to object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

# Rule to compile test files to object files
$(BUILD_DIR)/%.o: $(TEST_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CCFLAGS) -c $< -o $@

# Compile target
compile: CCFLAGS += -DSKIP_UNIT_TESTS="true"
compile: $(EXECUTABLE)

# Test target
test: CCFLAGS += -DRUN_UNIT_TESTS="true"
test: $(EXECUTABLE_TEST)

# Rule to build and run the unit tests and then generate a code coverage report
coverage: CCFLAGS += -fprofile-arcs -ftest-coverage
coverage: LDFLAGS += -lgcov
coverage: clean test
	$(BUILD_DIR)/main --test --all
	lcov --capture --directory . --output-file $(BUILD_DIR)/coverage.info
	genhtml $(BUILD_DIR)/coverage.info --output-directory $(BUILD_DIR)/coverage_html

autogen:
	python3.10 treemap_c.py -s src/tree.c --name "tree" --key-type "key_t" --value-type "data_t" --wipe --default-key "NULL" --default-value "NULL" --comparator "*X < *Y ? -1 : (*X > *Y ? +1 : 0)" -i "common.h" --deque

# Clean target
clean:
	rm -rf $(BUILD_DIR)/*.o $(EXECUTABLE) $(EXECUTABLE_TEST)

# Phony targets
.PHONY: all clean compile test coverage autogen
