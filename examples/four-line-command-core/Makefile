# Host build for the example only. This is not the production firmware build:
# the production PlatformIO configuration is deliberately not published.
#
# Requires a host C++17 compiler (g++ or clang++): Linux, macOS or WSL.

CXX ?= g++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra -Wpedantic
CPPFLAGS ?= -Iinclude
LDLIBS ?= -pthread

BUILD_DIR := build
SOURCES := $(wildcard src/*.cpp)
TEST_SOURCES := $(wildcard tests/*.cpp)
OBJECTS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SOURCES))
TEST_OBJECTS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(TEST_SOURCES))
TEST_BINARY := $(BUILD_DIR)/command-core-tests

.PHONY: all test clean

all: $(TEST_BINARY)

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

$(TEST_BINARY): $(OBJECTS) $(TEST_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDLIBS) -o $@

test: $(TEST_BINARY)
	./$(TEST_BINARY)

clean:
	rm -rf $(BUILD_DIR)
