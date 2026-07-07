CC := gcc

# Common paths
ROOT_DIR := ..
SRC_DIR := $(ROOT_DIR)/src
LOG_DIR := $(SRC_DIR)/log
TREE_DIR := $(SRC_DIR)/tree_2_3
HASHMAP_DIR := $(SRC_DIR)/hashmap

# Common flags
CFLAGS := -Wall -Wextra -std=c11 # -g -O1 -fsanitize=address -fno-omit-frame-pointer
CPPFLAGS := -I$(SRC_DIR)
LDLIBS := -lcheck -lsubunit -lpthread -lrt -lm

# Logging flags
LOG_DEFINES := -DLOG_USE_COLOR
TREE_DEFINES := -DNO_LOGGING
HASHMAP_DEFINES := -DNO_LOGGING

# Valgrind
VALGRIND := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1

# Tree test
TREE_SRC := $(TREE_DIR)/tree_2_3.c
TREE_TEST := ./test_tree_2_3.c
TREE_OBJ := ./tree_2_3.o
LOG_OBJ := ./log.o
TREE_BIN := ./test_tree_2_3
TREE_TEST_FLAGS := -D_XOPEN_SOURCE

# Hashmap test
HASHMAP_SRC := $(HASHMAP_DIR)/hashmap.c
HASHMAP_TEST := ./test_hashmap.c
HASHMAP_OBJ := ./hashmap.o
HASHMAP_BIN := ./test_hashmap
HASHMAP_TEST_FLAGS := -D_GNU_SOURCE

all: test-all

# Build tree objects
$(TREE_OBJ): $(TREE_SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TREE_DEFINES) -c $< -o $@

$(LOG_OBJ): $(LOG_DIR)/log.c
	$(CC) $(CFLAGS) $(CPPFLAGS) $(LOG_DEFINES) -c $< -o $@

$(TREE_BIN): $(TREE_OBJ) $(LOG_OBJ) $(TREE_TEST)
	$(CC) $(CFLAGS) $(TREE_TEST_FLAGS) $(CPPFLAGS) $^ -o $@ $(LDLIBS)

# Build hashmap objects
$(HASHMAP_OBJ): $(HASHMAP_SRC)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(HASHMAP_DEFINES) -c $< -o $@

$(HASHMAP_BIN): $(TREE_OBJ) $(HASHMAP_OBJ) $(LOG_OBJ) $(HASHMAP_TEST)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(HASHMAP_TEST_FLAGS) $^ -o $@ $(LDLIBS)

# Run targets
test-tree: $(TREE_BIN)
	./$(TREE_BIN)

test-hashmap: $(HASHMAP_BIN)
	./$(HASHMAP_BIN)

test-all: $(TREE_BIN) $(HASHMAP_BIN)
	./$(TREE_BIN)
	./$(HASHMAP_BIN)

# Memory test targets
test-tree-mem: $(TREE_BIN)
	$(VALGRIND) ./$(TREE_BIN)

test-hashmap-mem: $(HASHMAP_BIN)
	$(VALGRIND) ./$(HASHMAP_BIN)

test-all-mem: $(TREE_BIN) $(HASHMAP_BIN)
	$(VALGRIND) ./$(TREE_BIN)
	$(VALGRIND) ./$(HASHMAP_BIN)

clean:
	rm -f $(TREE_BIN) $(HASHMAP_BIN) *.o

re: clean all

.PHONY: all test-tree test-hashmap test-all test-tree-mem test-hashmap-mem test-all-mem clean re
