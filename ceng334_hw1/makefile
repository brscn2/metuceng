CC = g++
CFLAGS = -Wall -g

eshell: eshell.o parser.o
	$(CC) $(CFLAGS) -o eshell eshell.o parser.o

eshell.o: eshell.cpp parser.h
	$(CC) $(CFLAGS) -c eshell.cpp

parser.o: parser.h
