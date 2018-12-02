#pragma once

#include <swerve/lexer/lexer_error.h>
#include <swerve/lexer/token_queue.h>

typedef struct _LexerResult {
  enum {
    LEXER_ERROR,
    LEXER_OK,
  } type;
  TokenQueue* tokens;
  LexerError error;
} LexerResult;