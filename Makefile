CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude -Weffc++ 

SRC_DIR := src
INC_DIR := include
BUILD_DIR := builds
TEST_DIR := test

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DBG_OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.dbg.o,$(SRCS))
PERFT_SRC := $(TEST_DIR)/perft.cpp
PERFT_OBJ := $(BUILD_DIR)/perft.o

TARGET := $(BUILD_DIR)/main
DEBUG_TARGET := $(BUILD_DIR)/mainDebug
PERFT_TARGET := $(BUILD_DIR)/perft

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

$(PERFT_TARGET): $(PERFT_OBJ) $(filter-out $(BUILD_DIR)/main.o, $(OBJS))
	$(CXX) $(CXXFLAGS) -Ofast -o $@ $^

$(BUILD_DIR)/%.dbg.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -g -O0 -c $< -o $@

$(BUILD_DIR)/perft.o: $(PERFT_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -Ofast -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*.o $(BUILD_DIR)/*.dbg.o $(TARGET) $(DEBUG_TARGET)

run: release
	./$(TARGET)

run-debug: debug
	./$(DEBUG_TARGET)

run-perft: $(PERFT_TARGET)
	./$(PERFT_TARGET)

.PHONY: all release debug clean run run-debug

