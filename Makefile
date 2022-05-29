OBJS = slip.o lval.o mpc.o
CFLAGS = -g -Wall -std=c99
CC = cc
LIBS = -ledit
BUILD = build/slip

output: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $(BUILD)

slip.o: src/slip.c
	$(CC) $(CFLAGS) -c src/slip.c

lval.o: src/lval.c
	$(CC) -c src/lval.c

mpc.o: mpc/mpc.c mpc/mpc.h
	$(CC) -c mpc/mpc.c

clean:
	rm *.o

