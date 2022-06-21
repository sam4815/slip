#include <string.h>

int char_escapable(char c) {
  char* escapable = "\a\b\f\n\r\t\v\\\'\"";
  return strchr(escapable, c) != NULL;
};

int char_unescapable(char c) {
  char* unescapable = "abfnrtv\\\'\"";
  return strchr(unescapable, c) != NULL;
};

char char_unescape(char c) {
  switch (c) {
    case 'a':  return '\a';
    case 'b':  return '\b';
    case 'f':  return '\f';
    case 'n':  return '\n';
    case 'r':  return '\r';
    case 't':  return '\t';
    case 'v':  return '\v';
    case '\\': return '\\';
    case '\'': return '\'';
    case '\"': return '\"';
  }

  return '\0';
};

char* char_escape(char c) {
  switch (c) {
    case '\a':  return "\\a";
    case '\b':  return "\\b";
    case '\f':  return "\\f";
    case '\n':  return "\\n";
    case '\r':  return "\\r";
    case '\t':  return "\\t";
    case '\v':  return "\\v";
    case '\\': return "\\\\";
    case '\'': return "\\\'";
    case '\"': return "\\\"";
  }
  
  return "\0";
};
