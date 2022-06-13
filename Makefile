SRC = $(wildcard core/*.c) $(wildcard terminal/*.c) core/mpc/mpc.c
CFLAGS = -g -Wall -std=c99
CC = cc
LIBS = -ledit
TARGET = build/slip

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LIBS) $(OBJS) -o $@ $^

start: $(TARGET)
	$(TARGET)
