SRC = $(wildcard core/*.c) core/mpc/mpc.c
CFLAGS = -g -Wall -std=c99
CC = cc
LIBS = -lm
TERM_LIBS = -ledit
MAKE_TARGET_DIR = mkdir -p build
TERM_TARGET = build/slip
TEST_TARGET = build/test
WEB_TARGET = build/web

$(TERM_TARGET): $(SRC) $(wildcard terminal/*.c)
	$(MAKE_TARGET_DIR)
	$(CC) $(CFLAGS) $(TERM_LIBS) $(LIBS) -o $@ $^

$(WEB_TARGET): $(SRC) $(wildcard web/*.c)
	$(MAKE_TARGET_DIR)
	emcc $(CFLAGS) -o build/slip.js $^ -s "EXPORTED_RUNTIME_METHODS=['ccall']"

$(TEST_TARGET): $(SRC) $(wildcard test/*.c)
	$(MAKE_TARGET_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 

start: $(TERM_TARGET)
	@$(TERM_TARGET)

web: $(WEB_TARGET)
	cp web/* build

test: $(TEST_TARGET)
	@$(TEST_TARGET)
