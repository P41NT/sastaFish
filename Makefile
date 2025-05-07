CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude -Weffc++

SRC_DIR := src
INC_DIR := include
BUILD_DIR := builds

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DBG_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.dbg.o,$(SRCS))

TARGET := $(BUILD_DIR)/main
DEBUG_TARGET := $(BUILD_DIR)/mainDebug

all: release

release: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -Ofast -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -Ofast -c $< -o $@

debug: $(DEBUG_TARGET)

$(DEBUG_TARGET): $(DBG_OBJS)
	$(CXX) $(CXXFLAGS) -g -O0 -o $@ $^

$(BUILD_DIR)/%.dbg.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -g -O0 -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.dbg.o $(TARGET) $(DEBUG_TARGET)

run: release
	./$(TARGET)

run-debug: debug
	./$(DEBUG_TARGET)

.PHONY: all release debug clean run run-debug

