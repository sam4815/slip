#include "lval_parser.h"
#include "lval_operations.h"
#include "string_escaping.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

lval* parse_lval_number(char* part)
{
    errno = 0;
    long number = strtol(part, NULL, 10);
    return errno == ERANGE ? lval_err("Invalid number") : lval_num(number);
}

lval* parse_lval_bool(char* part)
{
    if (strstr(part, "true"))
        return lval_bool(1);
    if (strstr(part, "false"))
        return lval_bool(0);
    return lval_err("Invalid boolean");
}

int is_valid_boolean(char* part)
{
    return strstr(part, "true") || strstr(part, "false");
}

int is_valid_number(char* part)
{
    if (strchr("-", part[0]) && strlen(part) == 1) {
        return 0;
    }

    int is_number = strchr("-0123456789", part[0]) != NULL;
    for (int i = 1; i < strlen(part); i++) {
        is_number = strchr("0123456789", part[i]) != NULL;
    }

    return is_number;
}

int is_valid_symbol_char(char character)
{
    return strchr(
               "abcdefghijklmnopqrstuvwxyz"
               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
               "0123456789_+-*\\/=<>!&",
               character)
        != NULL;
}

int read_symbol(lval* parent, char* str, int i)
{
    char* part = calloc(1, 1);

    while (is_valid_symbol_char(str[i]) && str[i] != '\0') {
        part = realloc(part, strlen(part) + 2);
        part[strlen(part) + 1] = '\0';
        part[strlen(part)] = str[i];

        i = i + 1;
    }

    if (is_valid_number(part)) {
        append_lval(parent, parse_lval_number(part));
    } else if (is_valid_boolean(part)) {
        append_lval(parent, parse_lval_bool(part));
    } else {
        append_lval(parent, lval_sym(part));
    }

    free(part);

    return i;
};

int read_substring(lval* parent, char* str, int i)
{
    char* part = calloc(1, 1);

    while (str[i] != '"') {
        char c = str[i];

        if (c == '\0') {
            append_lval(parent, lval_err("Unexpected end of string input."));
            free(part);
            return strlen(str);
        }

        if (c == '\\') {
            i = i + 1;
            if (char_unescapable(str[i])) {
                c = char_unescape(str[i]);
            } else {
                append_lval(parent, lval_err("Invalid escape character %c.", str[i]));
                free(part);
                return strlen(str);
            }
        }

        part = realloc(part, strlen(part) + 2);
        part[strlen(part) + 1] = '\0';
        part[strlen(part)] = c;
        i = i + 1;
    }

    append_lval(parent, lval_str(part));

    free(part);

    return i + 1;
};

int lval_read_string(lval* parent, char* str, int i, char char_end)
{
    while (str[i] != char_end) {
        // If the char_end can't be found, the input string is invalid
        if (str[i] == '\0') {
            append_lval(parent, lval_err("Missing %c at end of input.", char_end));
            return strlen(str) + 1;
        }

        // Skip whitespace
        if (strchr(" \t\v\n\r", str[i]) != NULL) {
            i = i + 1;
            continue;
        }

        // Skip comments
        if (str[i] == ';') {
            while (strchr("\n\0", str[i]) == NULL) {
                i = i + 1;
            }
            i = i + 1;
            continue;
        }

        if (str[i] == '(') {
            lval* sexpr = lval_sexpr();
            append_lval(parent, sexpr);
            i = lval_read_string(sexpr, str, i + 1, ')');
            continue;
        }

        if (str[i] == '{') {
            lval* qexpr = lval_qexpr();
            append_lval(parent, qexpr);
            i = lval_read_string(qexpr, str, i + 1, '}');
            continue;
        }

        if (is_valid_symbol_char(str[i])) {
            i = read_symbol(parent, str, i);
            continue;
        }

        if (strchr("\"", str[i]) != NULL) {
            i = read_substring(parent, str, i + 1);
            continue;
        }

        append_lval(parent, lval_err("Unknown character %c.", str[i]));
        return strlen(str) + 1;
    }

    return i + 1;
}

lval* read_string(char* str)
{
    lval* result = lval_sexpr();

    int pos = 0;
    lval_read_string(result, str, pos, '\0');

    return result;
};
