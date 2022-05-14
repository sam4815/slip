#include <stdio.h>
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


int main(int argc, char** argv) {
	puts("slip v0.1\n");

	while (1) {
		char* input = readline("slip> ");
		add_history(input);
		
		printf("No you're a %s\n", input);
		free(input);
	}

	return 0;
}

