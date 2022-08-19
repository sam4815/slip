#include <stdlib.h>
#include <string.h>
#include "../core/slip.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt) {
	fputs(prompt, stdout);
	fgets(buffer, 2048, stdin);
	char* cpy = malloc(strlen(buffer) + 1);
	strcpy(cpy, buffer);
	cpy[strlen(copy) - 1] = '\0';
	return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#endif

#define YELLOW "\033[1;93m"
#define RESET "\033[0m"

void print_message(char* message) {
  if (strlen(message) == 0) { return; }
  
  printf(YELLOW "%s\n" RESET, message);
}

int main(int argc, char** argv) {
	slip* slippy = initialize_slip();

  // If passed slip file, load and evaluate it
  if (argc == 2) {
    char* input = malloc(strlen(argv[1]) + 14 + 1);
    sprintf(input, "load_file \"%s\"", argv[1]);

    print_message(slippy->evaluate_string(slippy->environment, input));

    free(input);
    return 0;
  }

	// Load history
	FILE* history_file;
	history_file = fopen("build/.repl_history", "a+");
	rewind(history_file);
	setlinebuf(history_file);
	char line[256];
	while (fgets(line, sizeof(line), history_file)) {
		add_history(line);
	}

  // Load REPL
	while (1) {
		char* input = readline("🍂 ");
		if (strlen(input) > 0) {
			fprintf(history_file, "%s\n", input);
			add_history(input);
		}

		print_message(slippy->evaluate_string(slippy->environment, input));

		free(input);
	}

  slippy->destroy(slippy->environment);
	
	return 0;
}
