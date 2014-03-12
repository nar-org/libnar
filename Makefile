SOURCES = libnar.c
OBJECTS = $(SOURCES:.c=.o)

CC      = gcc
AR      = ar
CFLAGS ?= -W -Wall -Wextra -Werror
LDFLAGS = -L. -lnar
LIBRARY = libnar.a

NAR         = nar
NAR_SOURCES = nar.c
NAR_OBJECTS = $(NAR_SOURCES:.c=.o)

#CFLAGS += -DDEBUG

all: $(SOURCES) $(LIBRARY) $(NAR)

$(NAR): $(NAR_OBJECTS) $(OBJECTS)
	$(CC) -o $@ $+

$(LIBRARY): $(OBJECTS)
	$(AR) rc -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) $(LIBRARY)
	rm -f $(NAR_OBJECTS) $(NAR)
