CC = gcc
CPPFLAGS=
CFLAGS = -Wall -std=c11 -g -fpic -Iinclude/
LDFLAGS= -Lbin/ -Wl,-rpath=./bin

all: parser liblist.so

parser: GPXParser.o LinkedListAPI.o include/helper.h
	$(CC) $(CFLAGS) $(LDFLAGS) -shared bin/GPXParser.o bin/LinkedListAPI.o -o ../libgpxparse.so -lxml2

GPXParser.o: src/GPXParser.c include/GPXParser.h include/helper.h
	$(CC) `xml2-config --cflags --libs` $(CFLAGS) -c src/GPXParser.c -o bin/GPXParser.o -lxml2

LinkedListAPI.o: src/LinkedListAPI.c include/LinkedListAPI.h
	$(CC) $(CFLAGS) -c -fpic src/LinkedListAPI.c -o bin/LinkedListAPI.o -lxml2

clean:
	rm -rf bin/runMe bin/*.o bin/*.so
