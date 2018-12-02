#include <swerve/lexer/token_type.h>

const char* token_type_to_cstr(TokenType type) {
  switch (type) {
    case LEX_REFERENCE_NAME:      return "REFERENCE_NAME";
    case LEX_TYPE_NAME:           return "TYPE_NAME";
    case LEX_EOL:                 return "EOL";
    case LEX_INDENT:              return "INDENT";
    case LEX_COLON:               return "COLON";
    case LEX_EQ:                  return "EQ";
    case LEX_COMMA:               return "COMMA";
    case LEX_RIGHT_ARROW:         return "LEX_RIGHT_ARROW";
    case LEX_WILDCARD:            return "LEX_WILDCARD";
    case LEX_OPEN_BRACKET:        return "OPEN_BRACKET";
    case LEX_CLOSE_BRACKET:       return "CLOSE_BRACKET";
    case LEX_OPEN_BRACE:          return "OPEN_BRACE";
    case LEX_CLOSE_BRACE:         return "CLOSE_BRACE";
    case LEX_OPEN_PAREN:          return "OPEN_PAREN";
    case LEX_CLOSE_PAREN:         return "CLOSE_PAREN";
    case LEX_INFIX_FUNCTION:      return "INFIX_FUNCTION_NAME";
    case LEX_AS:                  return "AS";
    case LEX_DATA:                return "DATA";
    case LEX_DEF:                 return "DEF";
    case LEX_FOR:                 return "FOR";
    case LEX_ELIF:                return "ELIF";
    case LEX_ELSE:                return "ELSE";
    case LEX_ENUM:                return "LEX_ENUM";
    case LEX_IF:                  return "IF";
    case LEX_IMPORT:              return "IMPORT";
    case LEX_INT:                 return "INT";
    case LEX_FLOAT:               return "FLOAT";
    case LEX_STRING:              return "STRING";
    case LEX_KEYWORD:             return "KEYWORD";
  }
}