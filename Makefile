CFLAGS=-O2 -std=c11 -Wno-strict-prototypes -pedantic -Wall -fcommon -g -D_POSIX_C_SOURCE=200809L
CHECK_FLAGS := $(shell bash check_flags.sh)

SRCDIR=./src
SRCLIST=$(wildcard $(SRCDIR)/*.c)
OBJLIST=$(SRCLIST:.c=.o)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

.PHONY: all clean

all: compressor

compressor:
	$(CC) $(CFLAGS) $(OBJLIST) -o compressor

check:
	$(CC) -O0 -o unittest tests/unit_tests.c -g $(filter-out src/main.c, $(wildcard src/*.c)) $(CHECK_FLAGS)
	@./unittest

check-huffman: 
	$(CC) -O0 -o huffmantest tests/huffman_tests.c -g $(filter-out src/main.c, $(wildcard src/*.c)) $(CHECK_FLAGS)
	@./huffmantest

codecov:
	$(CC) -ftest-coverage -coverage -g -fprofile-arcs -O0 -o unittest tests/unit_tests.c $(filter-out src/main.c, $(wildcard src/*.c)) $(CHECK_FLAGS)
	$(CC) -ftest-coverage -coverage -g -fprofile-arcs -O0 -o huffmantest tests/huffman_tests.c $(filter-out src/main.c, $(wildcard src/*.c)) $(CHECK_FLAGS)

coverage-html: codecov
	@./unittest
	@./huffmantest
	gcovr --exclude tests/ --exclude src/error.c --exclude src/fileread.c --exclude src/main.c --html-details coverage.html

format:
	@python ./format.py

clean:
	rm -f compressor $(OBJLIST) unittest *.gcda *.gcno *.gcov coverage.*
compressor: $(OBJLIST)
