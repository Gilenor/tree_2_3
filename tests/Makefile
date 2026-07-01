CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g -fsanitize=address -fno-omit-frame-pointer
INCLUDES = -I../ -I../src
DEFINES = -D_POSIX_C_SOURCE=200809L -DLOG_USE_COLOR -DLOGGING -DLOG_LEVEL_DEBUG
LDFLAGS = -lcheck -lsubunit -lpthread -lm

SRC = ../src/tree_2_3/tree_2_3.c ../src/log/log.c
TEST = ./test_tree_2_3.c

test_tree_2_3: $(SRC) $(TEST)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) $(SRC) $(TEST) -o $@ $(LDFLAGS)

test: test_tree_2_3
	./test_tree_2_3

clean:
	rm -f ./test_tree_2_3
