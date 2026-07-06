# compile tree obj file
gcc -Wall -Wextra -pedantic -g -O1 -fsanitize=address -fno-omit-frame-pointer \
-I../ \
-DLOG_LEVEL_ERROR \
-c \
../tree_2_3/tree_2_3.c \
-o tree_2_3.o \

# compile hashmap obj file
gcc -Wall -Wextra -pedantic -g -O1 -fsanitize=address -fno-omit-frame-pointer \
-I../ \
-DLOGGING -DLOG_LEVEL_TRACE \
-c \
./hashmap.c \
-o hashmap.o \

# compile test_hashmap executable file
gcc -Wall -Wextra -pedantic -g -O1 -fsanitize=address -fno-omit-frame-pointer \
-DLOGGING -DLOG_LEVEL_TRACE -DLOG_USE_COLOR \
-I../ \
./tree_2_3.o ./hashmap.o ../log/log.c ./hashmap_test.c \
-o test_hashmap
