CC = gcc
CFLAGS = -g -Wall
objects = RMA.o rma_main.o

all : rma_main

rma_main : $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

clean :
	rm rma_main *.o 
