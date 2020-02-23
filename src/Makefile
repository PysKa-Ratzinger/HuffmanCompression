SRC_DIR := ./src
OBJ_DIR := ./build
BIN_DIR := ./bin
TARGET  := main

dir_guard=@mkdir -p $(@D)

CC := g++

CXXFLAGS := -W
CXXFLAGS += -Wall
CXXFLAGS += -Wextra
CXXFLAGS += -Werror
CXXFLAGS += -Wfatal-errors
CXXFLAGS += -g

VALGRINDFLAGS := --leak-check=full
VALGRINDFLAGS += --show-leak-kinds=all
VALGRINDFLAGS += --track-origins=yes

SOURCES := $(wildcard $(SRC_DIR)/*.cpp)
OBJECTS := $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

target: $(OBJECTS) $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJECTS)
	$(dir_guard)
	$(CC) -o $@ $+ $(LDFLAGS)

$(OBJECTS): $(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	$(dir_guard)
	$(CC) -o $@ -c $(CXXFLAGS) $<

run: $(BIN_DIR)/$(TARGET)
	$(BIN_DIR)/$(TARGET)

debug: $(BIN_DIR)/$(TARGET)
	valgrind $(VALGRINDFLAGS) $(BIN_DIR)/$(TARGET)

clean:
	rm -rfv $(BIN_DIR) $(OBJ_DIR)

