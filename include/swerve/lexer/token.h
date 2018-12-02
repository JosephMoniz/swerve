#pragma once

#include <stdlib.h>

#include <swerve/lexer/token_type.h>

typedef struct _Token {
  TokenType type;
  const char* source;
  size_t offset;
  size_t length;
  size_t line;
  size_t column;
  union {
    struct {
      size_t level;
    } indent;
  };
} Token;

void token_print(const Token* token);