#include <stdio.h>
#include <string.h>

#include <swerve/lexer/lex.h>

int main() {
  
  const char* program =
    "variable = 123\n"
    "a = 1\n"
    "b = \"string\"\n"
    "c = True\n"
    "d = 3.14\n"
    "e = :keyword\n"
    "f = [1, 2, 3]\n"
    "g = {1, 2, 3}\n"
    "h = {\"a\": 1, \"b\": 2}\n"
    "a2: Num = 1\n"
    "b2: Str = \"string\"\n"
    "c2: Float = 3.14\n"
    "e2: Keyword = :keyword\n"
    "f2: List[Num] = [1, 2, 3]\n"
    "g2: Set[Num] = {1, 2, 3}\n"
    "h2: Map[Str, Num] = {\"a\": 1, \"b\": 2}\n"
    "\n"
    "m1 = [\n"
    "    1,\n"
    "    2,\n"
    "    3,\n"
    "    4,\n"
    "]\n"
    "m2: Seq[Num] = [\n"
    "    1,\n"
    "    2,\n"
    "    3,\n"
    "    4,\n"
    "]\n"
    "\n"
    "m3 = {\n"
    "    1,\n"
    "    2,\n"
    "    3,\n"
    "    4,\n"
    "}\n"
    "\n"
    "m4: Set[Num] = {\n"
    "    1,\n"
    "    2,\n"
    "    3,\n"
    "    4,\n"
    "}\n"
    "\n"
    "m5 = {\n"
    "    \"a\": 1,\n"
    "    \"b\": 2,\n"
    "    \"c\": 3,\n"
    "}\n"
    "\n"
    "m6: Map[Str, Num] = {\n"
    "    \"a\": 1,\n"
    "    \"b\": 2,\n"
    "    \"c\": 3,\n"
    "}\n"
    "\n"
    "cond1 = if boolean_value:\n"
    "    \"is_true\"\n"
    "else:\n"
    "    \"is_false\"\n"
    "\n"
    "cond2 = \"is_true\" if boolean_value else \"is_false\"\n"
    "\n"
    "cond3 = if first_value:\n"
    "    \"is_true\" if second_value else \"is_false\"\n"
    "else:\n"
    "    \"is_true\" if second_value else \"is_false\"\n"
    "\n"
    "cond4 = if first_value:\n"
    "    if second_value:\n"
    "        \"true\"\n"
    "    else:\n"
    "        \"false\"\n"
    "else:\n"
    "    if second_value:\n"
    "        \"true\"\n"
    "    else:\n"
    "        \"false\"\n"
    "\n"
    "enum Bool:\n"
    "    True\n"
    "    False\n"
    "\n"
    "A = TypeVar(\"A\")\n"
    "enum Optional:\n"
    "    None\n"
    "    Some(A)\n"
    "\n"
    "some1 = Some(\"existing\")\n"
    "\n"
    "some2 = Some(\n"
    "    \"existing\",\n"
    ")\n"
    "\n"
    "doubled = double(42)\n"
    "\n"
    "trippled = triple(\n"
    "    42,\n"
    ")\n"
    "\n"
    "halved = halve(number = 42)\n"
    "\n"
    "got = default_on_none(\n"
    "    optional = None,\n"
    "    default = 42,\n"
    ")\n"
    "\n"
    "data User:\n"
    "    name: Str\n"
    "    username: Str\n"
    "    password: Str\n"
    "\n"
    "user1 = User { \"Joseph\", \"jmoniz\", \"123\" }\n"
    "\n"
    "user2 = User {\n"
    "    \"Joseph\",\n"
    "    \"jmoniz\",\n"
    "    \"123\",\n"
    "}\n"
    "\n"
    "user3 = User { name: \"Joseph\", username: \"jmoniz\", password: \"123\" }\n"
    "\n"
    "user4 = User {\n"
    "    name: \"Joseph\",\n"
    "    username: \"jmoniz\",\n"
    "    password: \"123\",\n"
    "}\n"
    "\n"
    "def min(a: Num, b: Num) -> Num:\n"
    "    42\n"
    "\n"
    "A = TypeVar(\"A\")\n"
    "def min(a: A, b: B) -> B:\n"
    "    42\n"
    "\n"
    "def min(\n"
    "    a: Num,\n"
    "    b: Num,\n"
    ") -> Num:\n"
    "    42\n";

  const char* simple =
    "variable = reference\n"
    "typed: UInt32 = number\n"
    "if True:\n"
    "data def elif else enum \n"
    "string = \"value\"\n"
    "keyword = :keyword\n"
    "three = 3\n"
    "pi = 3.14\n";

  const char* running = program;

  LexerResult result = lex(running, strlen(running));
  TokenQueue* tokens = result.tokens;
  switch (result.type) {
    case LEXER_ERROR:
      printf(
        "# Lexer Error:\n"
        "%.*s\n"
        "%*s^\n"
        "%s\n"
        "At line: %d, column: %d\n",
        (int) (result.error.line_end - result.error.line_start),
        &running[result.error.line_start],
        (int) result.error.column,
        " ",
        result.error.message,
        (int) result.error.line,
        (int) result.error.column
      );
      break;
    case LEXER_OK:
      for (
        Token* token = token_queue_read(tokens);
        token;
        token = token_queue_read(tokens)
      ) {
        token_print(token);
      }
      break;
  }

  printf("Done!\n");

  return 0;

}