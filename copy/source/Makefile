TARGET=process_copy
MOD_TARGET=../mod/Copy
MOD_SRC=../mod/copy.c
INCLUDE_PATH=../include/
HEADER=$(INCLUDE_PATH)process_copy.h
LIBRARY_PATH=
SRCFILE=$(wildcard *.c)
DESTFILE=$(patsubst %.c,%.o,$(SRCFILE))
CFLAGS=-I$(INCLUDE_PATH) -g -Wall
CC=gcc

all: $(TARGET) $(MOD_TARGET)

$(TARGET):$(DESTFILE)
	$(CC) $^ -o $@

%.o:%.c $(HEADER)
	$(CC) $(CFLAGS) -c $<

$(MOD_TARGET): $(MOD_SRC)
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(DESTFILE) $(TARGET) $(MOD_TARGET)

.PHONY: all clean
