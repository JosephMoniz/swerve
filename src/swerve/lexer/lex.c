#include <stdbool.h>

#include <swerve/lexer/lex.h>

typedef struct _LexerIndentResult {
  enum {
    LEXER_INDENT_ERROR,
    LEXER_INDENT_FOUND,
    LEXER_INDENT_NOT_FOUND,
  } type;
  size_t next_offset;
  size_t next_column;
  struct { size_t level; } found;
} LexerIndentResult;

LexerIndentResult lex_indents(
  const char* in,
  size_t offset,
  size_t end,
  size_t column,
  size_t level
) {
  if (offset != end && in[offset] == ' ') {
    if (++offset != end && in[offset] == ' ' &&
        ++offset != end && in[offset] == ' ' &&
        ++offset != end && in[offset] == ' ') {
      return lex_indents(in, offset + 1, end, column + 4, level + 1);
    } else {
      return (LexerIndentResult) {
        .type = LEXER_INDENT_ERROR,
        .next_offset = offset,
        .next_column = column,
      };
    }
  } else {
    if (level) {
      return (LexerIndentResult) {
        .type = LEXER_INDENT_FOUND,
        .next_offset = offset,
        .next_column = column,
        .found = {
          .level = level,
        }
      };
    } else {
      return (LexerIndentResult) {
        .type = LEXER_INDENT_NOT_FOUND,
        .next_offset = offset,
        .next_column = column,
      };
    }
  }
}

typedef struct _SubLexResult {
  enum {
    LANGUAGE_LEXER_OK_EOL,
    LANGUAGE_LEXER_OK,
    LANGUAGE_LEXER_OK_NO_TOKEN,
    LANGUAGE_LEXER_ERROR,
  } type;
  union {
    struct {
      size_t offset;
      size_t column;
      const char* message;
    } error;
    struct {
      size_t next_offset;
      size_t next_column;
      TokenType token_type;
    } ok;
  };
} SubLexResult;

typedef SubLexResult (*Lexer)(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
);

static const Lexer lexers[256];
static const bool reference_characters[256];
static const bool type_characters[256];
static const bool infix_characters[256];
static const bool literal_term_characters[256];

SubLexResult lex_space(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  if (offset != end) {
    char c = in[offset];
    if (c == ' ' || c == '\t') {
      return lex_space(in, offset + 1, end, column + 1);
    }
  }
  return (SubLexResult) {
    .type = LANGUAGE_LEXER_OK_NO_TOKEN,
    .ok = {
      .next_offset = offset,
      .next_column = column ,
    }
  };
}

SubLexResult lex_eol(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  if (offset != end) {
    switch (in[offset]) {
      case '\r':
        return lex_eol(in, offset + 1, end, column + 1);
      case '\n':
        return (SubLexResult) {
          .type = LANGUAGE_LEXER_OK_EOL,
          .ok = {
            .next_offset = offset + 1,
            .next_column = column + 1,
            .token_type = LEX_EOL,
          }
        };
      default:
        break;
    }
  }
  return (SubLexResult) {
    .type = LANGUAGE_LEXER_ERROR,
    .error = {
      .offset = offset,
      .column = column,
      .message = "non line feed following a carriage return sequence",
    }
  };
}

SubLexResult lex_infix(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  if (offset != end && infix_characters[in[offset]]) {
    return lex_infix(in, offset + 1, end, column + 1);
  } else {
    return (SubLexResult) {
      .type = LANGUAGE_LEXER_OK,
      .ok = {
        .next_offset = offset,
        .next_column = column ,
        .token_type = LEX_INFIX_FUNCTION,
      }
    };
  }
}

SubLexResult lex_single_symbol(
  TokenType type,
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  size_t next_offset = offset + 1;
  if (next_offset == end || !infix_characters[in[next_offset]]) {
    return (SubLexResult) {
      .type = LANGUAGE_LEXER_OK,
      .ok = {
        .next_offset = next_offset,
        .next_column = column + 1,
        .token_type = type,
      }
    };
  } else {
    return lex_infix(in, next_offset, end, column + 1);
  }
}

SubLexResult lex_eq(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  return lex_single_symbol(LEX_EQ, in, offset, end, column);
}

SubLexResult lex_comma(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  return lex_single_symbol(LEX_COMMA, in, offset, end, column);
}

SubLexResult lex_wildcard(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  return lex_single_symbol(LEX_WILDCARD, in, offset, end, column);
}

SubLexResult lex_o_bracket(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  return lex_single_symbol(LEX_OPEN_BRACKET, in, offset, end, column);
}

SubLexResult lex_c_bracket(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  return lex_single_symbol(LEX_CLOSE_BRACKET, in, offset, end, column);
}

SubLexResult lex_o_brace(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  return lex_single_symbol(LEX_OPEN_BRACE, in, offset, end, column);
}

SubLexResult lex_c_brace(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  return lex_single_symbol(LEX_CLOSE_BRACE, in, offset, end, column);
}

SubLexResult lex_o_paren(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  return lex_single_symbol(LEX_OPEN_PAREN, in, offset, end, column);
}

SubLexResult lex_c_paren(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  return lex_single_symbol(LEX_CLOSE_PAREN, in, offset, end, column);
}

SubLexResult lex_keyword(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  if (offset != end && reference_characters[in[offset]]) {
    return lex_keyword(in, offset + 1, end, column + 1);
  } else {
    return (SubLexResult) {
      .type = LANGUAGE_LEXER_OK,
      .ok = {
        .next_offset = offset,
        .next_column = column ,
        .token_type = LEX_KEYWORD,
      }
    };
  }
}

SubLexResult lex_colon(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  size_t ra_offset = offset;
  if (++ra_offset != end && reference_characters[in[ra_offset]]) {
    return lex_keyword(in, ra_offset, end, column + 1);
  } else {
    return lex_single_symbol(LEX_COLON, in, offset, end, column);
  }
}

SubLexResult lex_right_arrow(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  size_t ra_offset = offset;
  if (++ra_offset != end && in[ra_offset] == '>' &&
      ++ra_offset != end && in[ra_offset] == ' ') {
    return (SubLexResult) {
      .type = LANGUAGE_LEXER_OK,
      .ok = {
        .next_offset = ra_offset,
        .next_column = column + 2,
        .token_type = LEX_RIGHT_ARROW,
      }
    };
  } else {
    return lex_infix(in, offset, end, column);
  }
}

SubLexResult lex_type(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  if (offset != end && type_characters[in[offset]]) {
    return lex_type(in, offset + 1, end, column + 1);
  } else {
    return (SubLexResult) {
      .type = LANGUAGE_LEXER_OK,
      .ok = {
        .next_offset = offset,
        .next_column = column ,
        .token_type = LEX_TYPE_NAME,
      }
    };
  }
}

SubLexResult lex_ref(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  if (offset != end && reference_characters[in[offset]]) {
    return lex_ref(in, offset + 1, end, column + 1);
  } else {
    return (SubLexResult) {
      .type = LANGUAGE_LEXER_OK,
      .ok = {
        .next_offset = offset,
        .next_column = column ,
        .token_type = LEX_REFERENCE_NAME,
      }
    };
  }
}

SubLexResult lex_float(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  if (offset != end) {
    switch (in[offset]) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        return lex_float(in, offset + 1, end, column + 1);
      default:
        if (!literal_term_characters[in[offset]]) {
          return (SubLexResult) {
            .type = LANGUAGE_LEXER_ERROR,
            .error = {
              .offset = offset,
              .column = column,
              .message = "invalid float termination",
            }
          };
        }
        break;
    }
  }
  return (SubLexResult) {
    .type = LANGUAGE_LEXER_OK,
    .ok = {
      .next_offset = offset,
      .next_column = column,
      .token_type = LEX_FLOAT,
    }
  };
}

SubLexResult lex_int(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  if (offset != end) {
    switch (in[offset]) {
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        return lex_int(in, offset + 1, end, column + 1);
      case '.':
        return lex_float(in, offset + 1, end, column + 1);
      default:
        if (!literal_term_characters[in[offset]]) {
          return (SubLexResult) {
            .type = LANGUAGE_LEXER_ERROR,
            .error = {
              .offset = offset,
              .column = column,
              .message = "invalid int termination",
            }
          };
        }
        break;
    }
  }
  return (SubLexResult) {
    .type = LANGUAGE_LEXER_OK,
    .ok = {
      .next_offset = offset,
      .next_column = column,
      .token_type = LEX_INT,
    }
  };
}

SubLexResult lex_inner_string(
  bool is_escaped,
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  size_t t_offset;
  if (offset != end) {
    if (is_escaped) {
      return lex_inner_string(false, in, offset + 1, end, column + 1);
    } else {
      switch (in[offset]) {
        case '\\':
          return lex_inner_string(true, in, offset + 1, end, column + 1);
        case '"':
          t_offset = offset + 1;
          if (t_offset != end && !literal_term_characters[in[t_offset]]) {
            return (SubLexResult) {
              .type = LANGUAGE_LEXER_ERROR,
              .error = {
                .offset = offset,
                .column = column,
                .message = "invalid string termination",
              }
            };
          }
          return (SubLexResult) {
            .type = LANGUAGE_LEXER_OK,
            .ok = {
              .next_offset = t_offset,
              .next_column = column + 1,
              .token_type = LEX_STRING,
            }
          };
        case '\n':
          return (SubLexResult) {
            .type = LANGUAGE_LEXER_ERROR,
            .error = {
              .offset = offset,
              .column = column,
              .message = "new line in string",
            }
          };
        default:
          return lex_inner_string(false, in, offset + 1, end, column + 1);
      }
    }
  } else {
    return (SubLexResult) {
      .type = LANGUAGE_LEXER_ERROR,
      .error = {
        .offset = offset,
        .column = column,
        .message = "unterminated string",
      }
    };
  }
}

SubLexResult lex_string(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  return lex_inner_string(false, in, offset + 1, end, column + 1);
}

SubLexResult lex_a_lang(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  size_t a_offset = offset;
  if (++a_offset != end && in[a_offset] == 's' &&
      ++a_offset != end && in[a_offset] == ' ') {
    return (SubLexResult) {
      .type = LANGUAGE_LEXER_OK,
      .ok = {
        .next_offset = a_offset,
        .next_column = column + 2,
        .token_type = LEX_AS,
      }
    };
  } else {
    return lex_ref(in, offset, end, column);
  }
}

SubLexResult lex_d_lang(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  size_t d_offset = offset + 1;
  if (d_offset != end) {
    switch (in[d_offset]) {
      case 'a':
        if (++d_offset != end && in[d_offset] == 't' &&
            ++d_offset != end && in[d_offset] == 'a' &&
            ++d_offset != end && in[d_offset] == ' ') {
          return (SubLexResult) {
            .type = LANGUAGE_LEXER_OK,
            .ok = {
              .next_offset = d_offset,
              .next_column = column + 4,
              .token_type = LEX_DATA,
            }
          };
        }
        break;
      case 'e':
        if (++d_offset != end && in[d_offset] == 'f' &&
            ++d_offset != end && in[d_offset] == ' ') {
          return (SubLexResult) {
            .type = LANGUAGE_LEXER_OK,
            .ok = {
              .next_offset = d_offset,
              .next_column = column + 3,
              .token_type = LEX_DEF,
            }
          };
        }
        break;
      default:
        break;
    }
  }
  return lex_ref(in, offset, end, column);
}

SubLexResult lex_e_lang(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  size_t e_offset = offset + 1;
  if (e_offset != end) {
    switch (in[e_offset]) {
      case 'l':
        if (++e_offset != end) {
          switch (in[e_offset]) {
            case 'i':
              if (++e_offset != end && in[e_offset] == 'f' &&
                  ++e_offset != end && in[e_offset] == ' ') {
                return (SubLexResult) {
                  .type = LANGUAGE_LEXER_OK,
                  .ok = {
                    .next_offset = e_offset,
                    .next_column = column + 4,
                    .token_type = LEX_ELIF,
                  }
                };
              }
              break;
            case 's':
              if (++e_offset != end && in[e_offset] == 'e' &&
                  ++e_offset != end && in[e_offset] == ' ') {
                return (SubLexResult) {
                  .type = LANGUAGE_LEXER_OK,
                  .ok = {
                    .next_offset = e_offset,
                    .next_column = column + 4,
                    .token_type = LEX_ELSE,
                  }
                };
              }
              break;
            default:
              break;
          }
        }
        break;
      case 'n':
        if (++e_offset != end && in[e_offset] == 'u' &&
            ++e_offset != end && in[e_offset] == 'm' &&
            ++e_offset != end && in[e_offset] == ' ') {
          return (SubLexResult) {
            .type = LANGUAGE_LEXER_OK,
            .ok = {
              .next_offset = e_offset,
              .next_column = column + 4,
              .token_type = LEX_ENUM,
            }
          };
        }
        break;
      default:
        break;
    }
  }
  return lex_ref(in, offset, end, column);
}

SubLexResult lex_f_lang(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  size_t f_offset = offset;
  if (++f_offset != end && in[f_offset] == 'o' &&
      ++f_offset != end && in[f_offset] == 'r' &&
      ++f_offset != end && in[f_offset] == ' ') {
    return (SubLexResult) {
      .type = LANGUAGE_LEXER_OK,
      .ok = {
        .next_offset = f_offset,
        .next_column = column + 3,
        .token_type = LEX_FOR,
      }
    };
  } else {
    return lex_ref(in, offset, end, column);
  }
}

SubLexResult lex_i_lang(
  const char* in,
  size_t offset,
  size_t end,
  size_t column
) {
  size_t i_offset = offset + 1;
  if (i_offset != end) {
    switch (in[i_offset]) {
      case 'f':
        if (++i_offset != end && in[i_offset] == ' ') {
          return (SubLexResult) {
            .type = LANGUAGE_LEXER_OK,
            .ok = {
              .next_offset = i_offset,
              .next_column = column + 2,
              .token_type = LEX_IF,
            }
          };
        }
        break;
      case 'm':
        if (++i_offset != end && in[i_offset] == 'p' &&
            ++i_offset != end && in[i_offset] == 'o' &&
            ++i_offset != end && in[i_offset] == 'r' &&
            ++i_offset != end && in[i_offset] == 't' &&
            ++i_offset != end && in[i_offset] == ' ') {
          return (SubLexResult) {
            .type = LANGUAGE_LEXER_OK,
            .ok = {
              .next_offset = i_offset,
              .next_column = column + 6,
              .token_type = LEX_IMPORT,
            }
          };
        }
        break;
      default:
        break;
    }
  }
  return lex_ref(in, offset, end, column);
}

static const Lexer lex_invalid = NULL;

static const Lexer lexers[256] = {
/* 0 nul            1 soh            2 stx            3 etx            */
  lex_invalid,      lex_invalid,     lex_invalid,     lex_invalid,
/* 4 eot            5 enq            6 ack            7 bel            */
  lex_invalid,      lex_invalid,     lex_invalid,     lex_invalid,
/* 8 bs             9 ht             10 nl            11 vt            */
  lex_invalid,      lex_invalid,     lex_eol,         lex_invalid,
/* 12 np            13 cr            14 so            15 si            */
  lex_invalid,      lex_eol,         lex_invalid,     lex_invalid,
/* 16 dle           17 dc1           18 dc2           19 dc3           */
  lex_invalid,      lex_invalid,     lex_invalid,     lex_invalid,
/* 20 dc4           21 nak           22 syn           23 etb           */
  lex_invalid,      lex_invalid,     lex_invalid,     lex_invalid,
/* 24 can           25 em            26 sub           27 esc           */
  lex_invalid,      lex_invalid,     lex_invalid,     lex_invalid,
/* 28 fs            29 gs            30 rs            31 us            */
  lex_invalid,      lex_invalid,     lex_invalid,     lex_invalid,
/* 32 sp            33  !            34  "            35  #            */
  lex_space,        lex_infix,       lex_string,      lex_infix,
/* 36  $            37  %            38  &            39  '            */
  lex_infix,        lex_infix,       lex_infix,       lex_infix,
/* 40  (            41  )            42  *            43  +            */
  lex_o_paren,      lex_c_paren,     lex_infix,       lex_infix,
/* 44  ,            45  -            46  .            47  /            */
  lex_comma,        lex_right_arrow, lex_infix,       lex_infix,
/* 48  0            49  1            50  2            51  3            */
  lex_int,          lex_int,         lex_int,         lex_int,
/* 52  4            53  5            54  6            55  7            */
  lex_int,          lex_int,         lex_int,         lex_int,
/* 56  8            57  9            58  :            59  ;            */
  lex_int,          lex_int,         lex_colon,       lex_infix,
/* 60  <            61  =            62  >            63  ?            */
  lex_infix,        lex_eq,          lex_infix,       lex_infix,
/* 64  @            65  A            66  B            67  C            */
  lex_infix,        lex_type,        lex_type,        lex_type,
/* 68  D            69  E            70  F            71  G            */
  lex_type,         lex_type,        lex_type,        lex_type,
/* 72  H            73  I            74  J            75  K            */
  lex_type,         lex_type,        lex_type,        lex_type,
/* 76  L            77  M            78  N            79  O            */
  lex_type,         lex_type,        lex_type,        lex_type,
/* 80  P            81  Q            82  R            83  S            */
  lex_type,         lex_type,        lex_type,        lex_type,
/* 84  T            85  U            86  V            87  W            */
  lex_type,         lex_type,        lex_type,        lex_type,
/* 88  X            89  Y            90  Z            91  [            */
  lex_type,         lex_type,        lex_type,        lex_o_bracket,
/* 92  \            93  ]            94  ^            95  _            */
  lex_infix,        lex_c_bracket,   lex_infix,       lex_wildcard,
/* 96  `            97  a            98  b            99  c            */
  lex_infix,        lex_a_lang,      lex_ref,         lex_ref,
/* 100  d           101  e           102  f           103  g           */
  lex_d_lang,       lex_e_lang,      lex_f_lang,      lex_ref,
/* 104  h           105  i           106  j           107  k           */
  lex_ref,          lex_i_lang,          lex_ref,         lex_ref,
/* 108  l           109  m           110  n           111  o           */
  lex_ref,          lex_ref,         lex_ref,         lex_ref,
/* 112  p           113  q           114  r           115  s           */
  lex_ref,          lex_ref,         lex_ref,         lex_ref,
/* 116  t           117  u           118  v           119  w           */
  lex_ref,          lex_ref,         lex_ref,         lex_ref,
/* 120  x           121  y           122  z           123  {           */
  lex_ref,          lex_ref,         lex_ref,         lex_o_brace,
/* 124  |           125  }           126  ~           127 del          */
  lex_ref,          lex_c_brace,     lex_infix,       lex_invalid
};

static const bool reference_characters[256] = {
/* 0 nul            1 soh            2 stx            3 etx            */
  false,            false,           false,           false,
/* 4 eot            5 enq            6 ack            7 bel            */
  false,            false,           false,           false,
/* 8 bs             9 ht             10 nl            11 vt            */
  false,            false,           false,           false,
/* 12 np            13 cr            14 so            15 si            */
  false,            false,           false,           false,
/* 16 dle           17 dc1           18 dc2           19 dc3           */
  false,            false,           false,           false,
/* 20 dc4           21 nak           22 syn           23 etb           */
  false,            false,           false,           false,
/* 24 can           25 em            26 sub           27 esc           */
  false,            false,           false,           false,
/* 28 fs            29 gs            30 rs            31 us            */
  false,            false,           false,           false,
/* 32 sp            33  !            34  "            35  #            */
  false,            false,           false,           false,
/* 36  $            37  %            38  &            39  '            */
  false,            false,           false,           false,
/* 40  (            41  )            42  *            43  +            */
  false,            false,           false,           false,
/* 44  ,            45  -            46  .            47  /            */
  false,            false,           false,           false,
/* 48  0            49  1            50  2            51  3            */
  false,            true,            true,            true,
/* 52  4            53  5            54  6            55  7            */
  true,             true,            true,            true,
/* 56  8            57  9            58  :            59  ;            */
  true,             true,            false,           false,
/* 60  <            61  =            62  >            63  ?            */
  false,            false,           false,           false,
/* 64  @            65  A            66  B            67  C            */
  false,            false,           false,           false,
/* 68  D            69  E            70  F            71  G            */
  false,            false,           false,           false,
/* 72  H            73  I            74  J            75  K            */
  false,            false,           false,           false,
/* 76  L            77  M            78  N            79  O            */
  false,            false,           false,           false,
/* 80  P            81  Q            82  R            83  S            */
  false,            false,           false,           false,
/* 84  T            85  U            86  V            87  W            */
  false,            false,           false,           false,
/* 88  X            89  Y            90  Z            91  [            */
  false,            false,           false,           false,
/* 92  \            93  ]            94  ^            95  _            */
  false,            false,           false,           true,
/* 96  `            97  a            98  b            99  c            */
  false,            true,            true,            true,
/* 100  d           101  e           102  f           103  g           */
  true,             true,            true,            true,
/* 104  h           105  i           106  j           107  k           */
  true,             true,            true,            true,
/* 108  l           109  m           110  n           111  o           */
  true,             true,            true,            true,
/* 112  p           113  q           114  r           115  s           */
  true,             true,            true,            true,
/* 116  t           117  u           118  v           119  w           */
  true,             true,            true,            true,
/* 120  x           121  y           122  z           123  {           */
  true,             true,            true,            false,
/* 124  |           125  }           126  ~           127 del          */
  false,            false,           false,           false,
};

static const bool type_characters[256] = {
/* 0 nul            1 soh            2 stx            3 etx            */
  false,            false,           false,           false,
/* 4 eot            5 enq            6 ack            7 bel            */
  false,            false,           false,           false,
/* 8 bs             9 ht             10 nl            11 vt            */
  false,            false,           false,           false,
/* 12 np            13 cr            14 so            15 si            */
  false,            false,           false,           false,
/* 16 dle           17 dc1           18 dc2           19 dc3           */
  false,            false,           false,           false,
/* 20 dc4           21 nak           22 syn           23 etb           */
  false,            false,           false,           false,
/* 24 can           25 em            26 sub           27 esc           */
  false,            false,           false,           false,
/* 28 fs            29 gs            30 rs            31 us            */
  false,            false,           false,           false,
/* 32 sp            33  !            34  "            35  #            */
  false,            false,           false,           false,
/* 36  $            37  %            38  &            39  '            */
  false,            false,           false,           false,
/* 40  (            41  )            42  *            43  +            */
  false,            false,           false,           false,
/* 44  ,            45  -            46  .            47  /            */
  false,            false,           false,           false,
/* 48  0            49  1            50  2            51  3            */
  false,            true,            true,            true,
/* 52  4            53  5            54  6            55  7            */
  true,             true,            true,            true,
/* 56  8            57  9            58  :            59  ;            */
  true,             true,            false,           false,
/* 60  <            61  =            62  >            63  ?            */
  false,            false,           false,           false,
/* 64  @            65  A            66  B            67  C            */
  false,            true,            true,            true,
/* 68  D            69  E            70  F            71  G            */
  true,             true,            true,            true,
/* 72  H            73  I            74  J            75  K            */
  true,             true,            true,            true,
/* 76  L            77  M            78  N            79  O            */
  true,             true,            true,            true,
/* 80  P            81  Q            82  R            83  S            */
  true,             true,            true,            true,
/* 84  T            85  U            86  V            87  W            */
  true,             true,            true,            true,
/* 88  X            89  Y            90  Z            91  [            */
  true,             true,            true,            false,
/* 92  \            93  ]            94  ^            95  _            */
  false,            false,           false,           true,
/* 96  `            97  a            98  b            99  c            */
  false,            true,            true,            true,
/* 100  d           101  e           102  f           103  g           */
  true,             true,            true,            true,
/* 104  h           105  i           106  j           107  k           */
  true,             true,            true,            true,
/* 108  l           109  m           110  n           111  o           */
  true,             true,            true,            true,
/* 112  p           113  q           114  r           115  s           */
  true,             true,            true,            true,
/* 116  t           117  u           118  v           119  w           */
  true,             true,            true,            true,
/* 120  x           121  y           122  z           123  {           */
  true,             true,            true,            false,
/* 124  |           125  }           126  ~           127 del          */
  false,            false,           false,           false,
};

static const bool infix_characters[256] = {
/* 0 nul            1 soh            2 stx            3 etx            */
  false,            false,           false,           false,
/* 4 eot            5 enq            6 ack            7 bel            */
  false,            false,           false,           false,
/* 8 bs             9 ht             10 nl            11 vt            */
  false,            false,           false,           false,
/* 12 np            13 cr            14 so            15 si            */
  false,            false,           false,           false,
/* 16 dle           17 dc1           18 dc2           19 dc3           */
  false,            false,           false,           false,
/* 20 dc4           21 nak           22 syn           23 etb           */
  false,            false,           false,           false,
/* 24 can           25 em            26 sub           27 esc           */
  false,            false,           false,           false,
/* 28 fs            29 gs            30 rs            31 us            */
  false,            false,           false,           false,
/* 32 sp            33  !            34  "            35  #            */
  false,            true,            false,           true,
/* 36  $            37  %            38  &            39  '            */
  true,             true,            true,            false,
/* 40  (            41  )            42  *            43  +            */
  false,            false,           true,            true,
/* 44  ,            45  -            46  .            47  /            */
  false,            true,            true,            true,
/* 48  0            49  1            50  2            51  3            */
  false,            false,           false,           false,
/* 52  4            53  5            54  6            55  7            */
  false,            false,           false,           false,
/* 56  8            57  9            58  :            59  ;            */
  false,            false,           true,            true,
/* 60  <            61  =            62  >            63  ?            */
  true,             true,            true,            true,
/* 64  @            65  A            66  B            67  C            */
  true,             false,           false,           false,
/* 68  D            69  E            70  F            71  G            */
  false,            false,           false,           false,
/* 72  H            73  I            74  J            75  K            */
  false,            false,           false,           false,
/* 76  L            77  M            78  N            79  O            */
  false,            false,           false,           false,
/* 80  P            81  Q            82  R            83  S            */
  false,            false,           false,           false,
/* 84  T            85  U            86  V            87  W            */
  false,            false,           false,           false,
/* 88  X            89  Y            90  Z            91  [            */
  false,            false,           false,           false,
/* 92  \            93  ]            94  ^            95  _            */
  true,             false,           true,            false,
/* 96  `            97  a            98  b            99  c            */
  true,             false,           false,           false,
/* 100  d           101  e           102  f           103  g           */
  false,            false,           false,           false,
/* 104  h           105  i           106  j           107  k           */
  false,            false,           false,           false,
/* 108  l           109  m           110  n           111  o           */
  false,            false,           false,           false,
/* 112  p           113  q           114  r           115  s           */
  false,            false,           false,           false,
/* 116  t           117  u           118  v           119  w           */
  false,            false,           false,           false,
/* 120  x           121  y           122  z           123  {           */
  false,            false,           false,           false,
/* 124  |           125  }           126  ~           127 del          */
  true,             false,           true,            false,
};

static const bool literal_term_characters[256] = {
/* 0 nul            1 soh            2 stx            3 etx            */
  false,            false,           false,           false,
/* 4 eot            5 enq            6 ack            7 bel            */
  false,            false,           false,           false,
/* 8 bs             9 ht             10 nl            11 vt            */
  false,            true,            true,            false,
/* 12 np            13 cr            14 so            15 si            */
  false,            false,           false,           false,
/* 16 dle           17 dc1           18 dc2           19 dc3           */
  false,            false,           false,           false,
/* 20 dc4           21 nak           22 syn           23 etb           */
  false,            false,           false,           false,
/* 24 can           25 em            26 sub           27 esc           */
  false,            false,           false,           false,
/* 28 fs            29 gs            30 rs            31 us            */
  false,            false,           false,           false,
/* 32 sp            33  !            34  "            35  #            */
  true,             true,            false,           true,
/* 36  $            37  %            38  &            39  '            */
  true,             true,            true,            true,
/* 40  (            41  )            42  *            43  +            */
  true,             true,            true,            true,
/* 44  ,            45  -            46  .            47  /            */
  true,             true,            true,            true,
/* 48  0            49  1            50  2            51  3            */
  false,            false,           false,           false,
/* 52  4            53  5            54  6            55  7            */
  false,            false,           false,           false,
/* 56  8            57  9            58  :            59  ;            */
  false,            false,           true,            true,
/* 60  <            61  =            62  >            63  ?            */
  true,             true,            true,            true,
/* 64  @            65  A            66  B            67  C            */
  true,            false,           false,           false,
/* 68  D            69  E            70  F            71  G            */
  false,            false,           false,           false,
/* 72  H            73  I            74  J            75  K            */
  false,            false,           false,           false,
/* 76  L            77  M            78  N            79  O            */
  false,            false,           false,           false,
/* 80  P            81  Q            82  R            83  S            */
  false,            false,           false,           false,
/* 84  T            85  U            86  V            87  W            */
  false,            false,           false,           false,
/* 88  X            89  Y            90  Z            91  [            */
  false,            false,           false,           true,
/* 92  \            93  ]            94  ^            95  _            */
  true,             true,            true,            true,
/* 96  `            97  a            98  b            99  c            */
  true,             false,           false,           false,
/* 100  d           101  e           102  f           103  g           */
  false,            false,           false,           false,
/* 104  h           105  i           106  j           107  k           */
  false,            false,           false,           false,
/* 108  l           109  m           110  n           111  o           */
  false,            false,           false,           false,
/* 112  p           113  q           114  r           115  s           */
  false,            false,           false,           false,
/* 116  t           117  u           118  v           119  w           */
  false,            false,           false,           false,
/* 120  x           121  y           122  z           123  {           */
  false,            false,           false,           true,
/* 124  |           125  }           126  ~           127 del          */
  true,             true,            true,            false,
};

size_t find_eol(const char* in, size_t offset) {
  for (
    char n = in[offset];
    n && n != '\n';
    n = in[++offset]
  );
  return offset;
}

LexerResult lex(const char* in, size_t end) {
  size_t offset = 0;
  size_t line_start = 0;
  size_t line = 0;
  size_t column;
  TokenQueue* tokens = make_token_queue();

  // lex the source file line by line
  //

  while (offset != end) {

    line++;
    column = 1;

    // lex the indentation token
    //

    LexerIndentResult indent_result = lex_indents(in, offset, end, column, 0);

    switch (indent_result.type) {
      case LEXER_INDENT_ERROR:
        return (LexerResult) {
          .type = LEXER_ERROR,
          .tokens = tokens,
          .error = {
            .offset = indent_result.next_offset,
            .line_start = line_start,
            .line_end = find_eol(in, offset),
            .line = line,
            .column = indent_result.next_column,
            .message = "invalid indentation, 4 spaces required",
          }
        };
      case LEXER_INDENT_FOUND:
        token_queue_write(
          tokens,
          (Token) {
            .type = LEX_INDENT,
            .source = in,
            .offset = offset,
            .length = indent_result.next_offset - offset,
            .line = line,
            .column = column,
            .indent = {
              .level = indent_result.found.level
            },
          }
        );
      case LEXER_INDENT_NOT_FOUND:
        offset = indent_result.next_offset;
        column = indent_result.next_column;
    }

    // lex the language tokens for the line
    //

    bool line_ended = false;
    while (offset != end && !line_ended) {
      Lexer lexer = lexers[in[offset]];
      if (lexer) {
        SubLexResult result = lexer(in, offset, end, column);
        switch (result.type) {
          case LANGUAGE_LEXER_OK_EOL:
            line_start = result.ok.next_offset;
            line_ended = true;
          case LANGUAGE_LEXER_OK:
            token_queue_write(
              tokens,
              (Token) {
                .type = result.ok.token_type,
                .source = in,
                .offset = offset,
                .length = result.ok.next_offset - offset,
                .line = line,
                .column = column,
              }
            );
          case LANGUAGE_LEXER_OK_NO_TOKEN:
            offset = result.ok.next_offset;
            column = result.ok.next_column;
            break;
          case LANGUAGE_LEXER_ERROR:
            return (LexerResult) {
              .type = LEXER_ERROR,
              .tokens = tokens,
              .error = {
                .offset = result.error.offset,
                .line_start = line_start,
                .line_end = find_eol(in, offset),
                .line = line,
                .column = result.error.column,
                .message = result.error.message,
              },
            };
        }
      } else {
        return (LexerResult) {
          .type = LEXER_ERROR,
          .tokens = tokens,
          .error = {
            .offset = offset,
            .line_start = line_start,
            .line_end = find_eol(in, offset),
            .line = line,
            .column = column,
            .message = "invalid character",
          },
        };
      }
    }
  }

  return (LexerResult) {
    .type = LEXER_OK,
    .tokens = tokens,
  };

}