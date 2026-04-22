CXX      := g++
CXXFLAGS := -Wall -std=c++17 -I src -I lib/raylib/include
LDFLAGS  := -L lib/raylib/lib -lraylib

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin
TARGET  := $(BIN_DIR)/nimonspoli

ifeq ($(OS),Windows_NT)
    LDFLAGS += -lopengl32 -lgdi32 -lwinmm
else
    LDFLAGS += -lGL -lm -lpthread -ldl -lrt -lX11
    TARGET  := $(BIN_DIR)/nimonspoli
endif

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

all: dirs $(TARGET)

dirs:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "Build OK -> $(TARGET)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

rebuild: clean all

.PHONY: all dirs run clean rebuild