# An admittedly primitive Makefile
# To compile, type "make" or make "all"
# To remove files, type "make clean"

CC = gcc
CFLAGS = -Wall
OBJS = wserver.o wclient.o request.o io_helper.o 

.SUFFIXES: .c .o 

all: wserver wclient spin.cgi

wserver: wserver.o request.o io_helper.o
	$(CC) $(CFLAGS) -o wserver wserver.o request.o io_helper.o 

wclient: wclient.o io_helper.o
	$(CC) $(CFLAGS) -o wclient wclient.o io_helper.o

spin.cgi: spin.c
	$(CC) $(CFLAGS) -o spin.cgi spin.c

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	-rm -f $(OBJS) wserver wclient spin.cgi request_buffer_test request_buffer.o request_buffer_test.o

CFLAGS = -Wall -pthread

buffer_test: request_buffer_test.o request_buffer.o
	$(CC) $(CFLAGS) -o request_buffer_test request_buffer_test.o request_buffer.o

# optional: add buffer_test to all, or build explicitly
