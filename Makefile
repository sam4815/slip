OBJS = run.o slip.o lval.o lenv.o num_functions.o qexpr_functions.o mpc.o
CFLAGS = -g -Wall -std=c99
CC = cc
LIBS = -ledit
BUILD = build/slip

output: $(OBJS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $(BUILD)

%.o: core/%.c
	$(CC) $(CFLAGS) -c $<

mpc.o: core/mpc/mpc.c core/mpc/mpc.h
	$(CC) -c core/mpc/mpc.c

run.o: terminal/run.c
	$(CC) $(CFLAGS) -c terminal/run.c

clean:
	rm *.o
