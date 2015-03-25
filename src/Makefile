# A make file for the priority queue
#TODO or use -pthread ?

CC = gcc
CFLAGS = -I. -Wall
LDFLAGS = -lpthread
DFLAGS = -DSTACK_DEBUG -DCON_DEBUG
OBJS = main.o procon.o stack.o basic_time.o

queue.exe : $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJS) -o queue.exe

%.o : %.c
	$(CC) $(CFLAGS) $(DFLAGS) -c $<

.PHONY : clean
clean :
	-rm $(OBJS)

