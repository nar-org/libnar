SOURCES = libnar.c
OBJECTS = $(SOURCES:.c=.o)

CC      = gcc
AR      = ar
CFLAGS ?= -W -Wall -Wextra -Werror
LDFLAGS = -L. -lnar
LIBRARY = libnar.a

NAR         = nar
NAR_SOURCES = nar.c default_reader.c zlib_readers.c
NAR_OBJECTS = $(NAR_SOURCES:.c=.o)

CFLAGS += -DDEBUG

all: $(SOURCES) $(LIBRARY) $(NAR)

$(NAR): $(NAR_OBJECTS) $(OBJECTS)
	$(CC) -o $@ $+ -lz

$(LIBRARY): $(OBJECTS)
	$(AR) rc $@ $+

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(LIBRARY)
	rm -f $(NAR_OBJECTS) $(NAR)
	rm -f tests/test.nar tests/file2.txt

.PHONY: tests
tests: $(NAR)
	./$(NAR) -n tests/test.nar -c
	./$(NAR) -n tests/test.nar -a tests/file1.txt
	./$(NAR) -n tests/test.nar -a LICENSE
	./$(NAR) -n tests/test.nar -a README.md
	./$(NAR) -n tests/test.nar -e tests/file1.txt > tests/file2.txt && diff tests/file1.txt tests/file2.txt
