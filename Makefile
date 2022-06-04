OBJS = slip.o lval.o lenv.o qexpr_functions.o mpc.o
CFLAGS = -g -Wall -std=c99
CC = cc
LIBS = -ledit
BUILD = build/slip

output: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $(BUILD)

%.o: src/%.c
	$(CC) $(CFLAGS) -c $<

mpc.o: mpc/mpc.c mpc/mpc.h
	$(CC) -c mpc/mpc.c

clean:
	rm *.o
