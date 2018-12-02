#pragma once

typedef enum _TokenType {

  // Symbol Tokens
  //

  LEX_REFERENCE_NAME,
  LEX_TYPE_NAME,

  // Whitespace Tokens
  //

  LEX_EOL,
  LEX_INDENT,

  // Symbol Tokens
  //

  LEX_COLON,
  LEX_EQ,
  LEX_COMMA,
  LEX_RIGHT_ARROW,
  LEX_WILDCARD,
  LEX_OPEN_BRACKET,
  LEX_CLOSE_BRACKET,
  LEX_OPEN_BRACE,
  LEX_CLOSE_BRACE,
  LEX_OPEN_PAREN,
  LEX_CLOSE_PAREN,
  LEX_INFIX_FUNCTION,

  // Language Keyword Tokens
  //

  LEX_AS,
  LEX_DATA,
  LEX_DEF,
  LEX_ELIF,
  LEX_ELSE,
  LEX_ENUM,
  LEX_FOR,
  LEX_IF,
  LEX_IMPORT,

  // Literal Tokens
  //

  LEX_INT,
  LEX_FLOAT,
  LEX_STRING,
  LEX_KEYWORD,

} TokenType;

const char* token_type_to_cstr(TokenType type);