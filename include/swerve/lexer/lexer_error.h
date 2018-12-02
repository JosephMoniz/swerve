#pragma once

#include <stdlib.h>

typedef struct _LexerError {
  size_t offset;
  size_t line_start;
  size_t line_end;
  size_t line;
  size_t column;
  const char* message;
} LexerError;