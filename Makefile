SRC = $(wildcard core/*.c)
CFLAGS = -g -Wall -std=c99
CC = cc
LIBS = -lm
TERM_LIBS = -ledit
MAKE_TARGET_DIR = mkdir -p build
COPY_LIBRARY = cp core/library.slip build
TERM_TARGET = build/slip
TEST_TARGET = build/test
WEB_TARGET = build/web

$(TERM_TARGET): $(SRC) $(wildcard terminal/*.c)
	@$(MAKE_TARGET_DIR)
	@$(CC) $(CFLAGS) $(TERM_LIBS) $(LIBS) -o $@ $^

$(WEB_TARGET): $(SRC) $(wildcard web/*.c)
	@$(MAKE_TARGET_DIR)
	emcc $(CFLAGS) -o build/slip.js $^ -s "EXPORTED_RUNTIME_METHODS=['ccall']" --preload-file build

$(TEST_TARGET): $(SRC) $(wildcard test/*.c)
	@$(MAKE_TARGET_DIR)
	@$(CC) $(CFLAGS) -o $@ $^ $(LIBS) 

start: $(TERM_TARGET)
	@$(COPY_LIBRARY)
	@$(TERM_TARGET)

web: $(WEB_TARGET)
	cp web/* build
	@$(COPY_LIBRARY)

test: $(TEST_TARGET)
	@$(COPY_LIBRARY)
	@$(TEST_TARGET)
