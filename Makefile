CC = gcc
CFLAGS = -Wall -Werror -pedantic
OBJ = Logger.o ReadyQueue.o Scheduler.o
LIB = -lm -lpthread
EXEC = scheduler

$(EXEC) : $(OBJ)
	$(CC) $(OBJ) $(LIB) -o $(EXEC)

debug : CFLAGS += -DDEBUG=1
debug : clean $(OBJ)
	$(CC) $(OBJ) $(LIB) -g -DDEBUG=1 -o $(EXEC)

ReadyQueue.o : ReadyQueue.c ReadyQueue.c
	$(CC) -c ReadyQueue.c $(CFLAGS) $(LIB)

Logger.o : Logger.c Logger.h
	$(CC) -c Logger.c $(CFLAGS) $(LIB)

Scheduler.o : Scheduler.c Scheduler.h ReadyQueue.h Logger.h
	$(CC) -c Scheduler.c $(CFLAGS) $(LIB)

clean :
	rm -f $(OBJ) $(EXEC)

generator : Generator.c
	$(CC) $(CFLAGS) Generator.c -o generator
