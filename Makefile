CC := gcc
# debug
#CFLAGS := -Wall -Wextra -Werror -Wswitch-enum -std=c11 -pedantic -ggdb
# release
CFLAGS := -Wall -Wextra -Werror -Wswitch-enum -std=c11 -pedantic
LDLIBS := 
LDFLAGS := 
TARGET := ccalc

SRC_DIR := src
OBJ_DIR	:= obj
BIN_DIR	:= bin

TEST := "  100,53 + (3.5 - 5) + (4 *   6,4) /   8.3  "

EXE := $(BIN_DIR)/$(TARGET)
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# TODO: Implement debug and release builds.

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

check: $(EXE)
	valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all $^ $(TEST)

clean:
	@$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

remake: clean all

run: $(EXE)
	./$(EXE) $(TEST)

.PHONY: all check clean

-include $(OBJ:.o=.d)
