CC := gcc
CFLAGS := -Wall -Wextra -Wpedantic -Wswitch-enum -std=c11 # -Werror -> Treat all warnings as errors
LDLIBS := -lm
LDFLAGS := 
INCLUDE_DIRS := -Ithirdparty
TARGET := ccalc

SRC_DIR := src
OBJ_DIR	:= obj
BIN_DIR	:= bin

TEST_INPUT := "  100.53 + sqrt(3.5 - EN) + cos(44.23 *   6.4^2) /   8.3 + ln(10) - PI + ln(5^EC)"

EXE := $(BIN_DIR)/$(TARGET)
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(EXE)

debug: CFLAGS += -ggdb
debug: all

release: CFLAGS += -O3
release: all

remake: clean all

remakeDebug: clean debug

remakeRelease: clean release

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(INCLUDE_DIRS) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

check1: $(EXE)
	valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all $^ input -i $(TEST_INPUT) -v

check2: $(EXE)
	valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all $^ input $(TEST_INPUT) -v

check3: $(EXE)
	valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all $^ input $(TEST_INPUT)

check4: $(EXE)
	valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all $^ test -v

test1: $(EXE)
	$^ test -v
	
test2: $(EXE)
	$^ system-test -v

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

run: $(EXE)
	./$(EXE) input -v $(TEST_INPUT)

.PHONY: all check1 check2 check3 test1 test2 clean remake remakeDebug remakeRelease debug release run

-include $(OBJ:.o=.d)
