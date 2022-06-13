SRC = $(wildcard core/*.c) core/mpc/mpc.c
CFLAGS = -g -Wall -std=c99
CC = cc
LIBS = -ledit
TERM_TARGET = build/slip
TEST_TARGET = build/test

$(TERM_TARGET): $(SRC) $(wildcard terminal/*.c)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $@ $^

$(TEST_TARGET): $(SRC) $(wildcard test/*.c)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $@ $^

start: $(TERM_TARGET)
	@$(TERM_TARGET)

test: $(TEST_TARGET)
	@$(TEST_TARGET)
