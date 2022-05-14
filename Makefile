OBJS = slip.o mpc.o
CFLAGS = -Wall -std=c99 -ledit
CC = cc
BUILD = build/slip

output: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BUILD)

slip.o: slip.c
	$(CC) -c slip.c

mpc.o: mpc/mpc.c mpc/mpc.h
	$(CC) -c mpc/mpc.c  

clean:
	rm *.o

