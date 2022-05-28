OBJS = slip.o lval.o mpc.o
CFLAGS = -Wall -std=c99 -ledit
CC = cc
BUILD = build/slip

output: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(BUILD)

slip.o: src/slip.c
	$(CC) -c src/slip.c

lval.o: src/lval.c
	$(CC) -c src/lval.c

mpc.o: mpc/mpc.c mpc/mpc.h
	$(CC) -c mpc/mpc.c  

clean:
	rm *.o

