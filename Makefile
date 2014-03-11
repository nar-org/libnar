## Makefile for Project in libnar
##
## Made by Nicolas DI PRIMA
## Login   <nicolas@di-prima.fr>
##
## Started on  Tue 11 Mar 2014 13:39:01 GMT Nicolas DI PRIMA
## Last update Tue 11 Mar 2014 21:20:52 GMT Nicolas DI PRIMA

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
