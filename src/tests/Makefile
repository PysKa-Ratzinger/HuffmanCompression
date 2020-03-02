TEST_SRC_DIR := ./src
TEST_OBJ_DIR := ./build
TEST_BIN_DIR := ./bin

OBJ_DIR := ../build

TARGET  := main

dir_guard=@mkdir -p $(@D)

CC := g++

CXXFLAGS := -std=c++11
CXXFLAGS += -Wall
CXXFLAGS += -Werror
CXXFLAGS += -Wextra
CXXFLAGS += -Wformat-security
CXXFLAGS += -g
CXXFLAGS += -O0

LDLIBS := -lpthread
LDLIBS += -ldl
LDLIBS += -lrt
LDLIBS += -lcppunit
LDLIBS += -lm

TEST_SOURCES := $(wildcard $(TEST_SRC_DIR)/*.cpp $(TEST_SRC_DIR)/**/*.cpp)
OBJECTS_TEST := $(TEST_SOURCES:$(TEST_SRC_DIR)/%.cpp=$(TEST_OBJ_DIR)/%.o)

OBJECTS_REAL := $(wildcard $(OBJ_DIR)/*.o $(OBJ_DIR)/*/*.o)
OBJECTS_REAL := $(filter-out $(OBJ_DIR)/main.o, $(OBJECTS_REAL))

target: $(OBJECTS) $(TEST_BIN_DIR)/$(TARGET)

$(TEST_BIN_DIR)/$(TARGET): $(OBJECTS_TEST) $(OBJECTS_REAL)
	$(dir_guard)
	$(CC) -o $(TEST_BIN_DIR)/$(TARGET) $+ $(CXXFLAGS) $(LDLIBS)

$(OBJECTS_TEST): $(TEST_OBJ_DIR)/%.o : $(TEST_SRC_DIR)/%.cpp
	$(dir_guard)
	$(CC) -o $@ -I../src/ -c $(CXXFLAGS) $<

run: $(TEST_BIN_DIR)/$(TARGET)
	$(TEST_BIN_DIR)/$(TARGET)

clean:
	rm -rvf $(TEST_BIN_DIR) $(TEST_OBJ_DIR)

