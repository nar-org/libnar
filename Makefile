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
	$(CC) -lz -o $@ $+

$(LIBRARY): $(OBJECTS)
	$(AR) rc $@ $+

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(LIBRARY)
	rm -f $(NAR_OBJECTS) $(NAR)
