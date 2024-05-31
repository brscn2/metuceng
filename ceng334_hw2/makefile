CC = g++
CFLAGS = -lpthread

all: simulator.o WriteOutput.o helper.o 
	$(CC) $(CFLAGS) -o simulator simulator.o WriteOutput.o helper.o

simulator.o: simulator.cpp
	$(CC) $(CFLAGS) -c simulator.cpp

WriteOutput.o: WriteOutput.h
	$(CC) $(CFLAGS) -c WriteOutput.c

helper.o: helper.h
	$(CC) $(CFLAGS) -c helper.c

clean :
	-rm *.o $(objects) simulator
