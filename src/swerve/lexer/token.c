#include <swerve/lexer/token.h>
#include <printf.h>

void token_print(const Token* token) {
  switch (token->type) {
    case LEX_INDENT:
      printf(
        "Token {\n"
        "    type: \"%s\",\n"
        "    source: 0x%x,\n"
        "    offset: %d,\n"
        "    length: %d,\n"
        "    line: %d,\n"
        "    column: %d,\n"
        "    value: \"%.*s\",\n"
        "    level: %d\n"
        "}\n",
        token_type_to_cstr(token->type),
        (unsigned int) token->source,
        (int) token->offset,
        (int) token->length,
        (int) token->line,
        (int) token->column,
        (int) token->length,
        &token->source[token->offset],
        (int) token->indent.level
      );
      break;
    case LEX_EOL:
      printf(
        "Token {\n"
        "    type: \"%s\",\n"
        "    source: 0x%x,\n"
        "    offset: %d,\n"
        "    length: %d,\n"
        "    line: %d,\n"
        "    column: %d,\n"
        "}\n",
        token_type_to_cstr(token->type),
        (unsigned int) token->source,
        (int) token->offset,
        (int) token->length,
        (int) token->line,
        (int) token->column
      );
      break;
    case LEX_STRING:
      printf(
        "Token {\n"
        "    type: \"%s\",\n"
        "    source: 0x%x,\n"
        "    offset: %d,\n"
        "    length: %d,\n"
        "    line: %d,\n"
        "    column: %d,\n"
        "    value: %.*s,\n"
        "}\n",
        token_type_to_cstr(token->type),
        (unsigned int) token->source,
        (int) token->offset,
        (int) token->length,
        (int) token->line,
        (int) token->column,
        (int) token->length,
        &token->source[token->offset]
      );
      break;
    default:
      printf(
        "Token {\n"
        "    type: \"%s\",\n"
        "    source: 0x%x,\n"
        "    offset: %d,\n"
        "    length: %d,\n"
        "    line: %d,\n"
        "    column: %d,\n"
        "    value: \"%.*s\",\n"
        "}\n",
        token_type_to_cstr(token->type),
        (unsigned int) token->source,
        (int) token->offset,
        (int) token->length,
        (int) token->line,
        (int) token->column,
        (int) token->length,
        &token->source[token->offset]
      );
      break;
  }
}