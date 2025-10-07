CC = gcc
CFlags =-g -Wall -Wextra -Werror -std=c11
LDFlags = -lm
EXE = 


all: build

build: my_grep

my_grep: grep.c common.o
	clang-format -i *.c *.h
	$(CC) $(CFlags) $^ -o $@

common.o: ../common/common.c
	$(CC) -c $(CFlags) $^ -o $@

clean:
	rm -rf *.o *.cmp my_grep 
clang:
	clang-format -i *.c *.h ../common/*.c ../common/*.h