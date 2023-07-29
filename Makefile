CC=gcc
CFLAGS=-O2 -std=c11 -pedantic -Werror -Wno-unused-variable -Wno-unused-but-set-variable -Wall -fcommon

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
	$(CC) -O0 -o unittest tests/unit_tests.c -lcheck $(filter-out src/main.c, $(wildcard src/*.c)) $(CHECK_FLAGS)
	@./unittest

codecov:
	$(CC) -ftest-coverage -coverage -g -fprofile-arcs -O0 -o unittest tests/unit_tests.c $(filter-out src/main.c, $(wildcard src/*.c)) $(CHECK_FLAGS)

coverage-html: codecov
	@./unittest
	gcovr --html-details coverage.html

format:
	@python format.py	

clean:
	rm -f compressor $(OBJLIST) unittest *.gcda *.gcno *.gcov coverage.*
compressor: $(OBJLIST)
