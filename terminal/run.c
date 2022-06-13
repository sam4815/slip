#include "../core/mpc/mpc.h"
#include "../core/lval_definitions.h"
#include "../core/environment.h"
#include "../core/slip.h"
#include <stdlib.h>

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

int main(int argc, char** argv) {
	slip* slippy = initialize_slip();

	while (1) {
		char* input = readline("🍂 ");
		if (strlen(input) > 0) { add_history(input); }

		char* message = slippy->evaluate_string(slippy->parser, slippy->environment, input);

    printf(YELLOW "%s\n" RESET, message);

		free(input);
	}

  slippy->destroy(slippy->parser, slippy->environment);
	
	return 0;
}
