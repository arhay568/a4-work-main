/**
 * Tokenizer for a simple shell language.
 *
 * Tokens:
 *   ( ) < > ; |      single-character special tokens
 *   "..."            a quoted string; special characters lose their meaning
 *   word             a run of non-special, non-whitespace characters
 *
 * Whitespace (space, tab, newline) separates tokens but is never part of
 * a token unless it appears inside quotes.
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "tokens.h"

/** Is ch one of the single-character special tokens? */
static int is_special(char ch) {
  return ch != '\0' && strchr("()<>;|", ch) != NULL;
}

/** Is ch a whitespace separator? */
static int is_whitespace(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n';
}

/** Does ch end a word? (end of input, whitespace, special char, or quote) */
static int ends_word(char ch) {
  return ch == '\0' || is_whitespace(ch) || is_special(ch) || ch == '"';
}

/** Add the first len characters of start to the vector as one token. */
static void add_token(vect_t *tokens, const char *start, size_t len) {
  char *buf = malloc(len + 1);
  assert(buf != NULL);
  memcpy(buf, start, len);
  buf[len] = '\0';

  vect_add(tokens, buf); /* the vector stores its own copy */
  free(buf);
}

/** Read a quoted string starting just after the opening quote. Adds the
 *  contents (without quotes) as a token and returns the number of input
 *  characters consumed, including the closing quote if present. */
static size_t read_quoted(vect_t *tokens, const char *input) {
  size_t len = 0;
  while (input[len] != '\0' && input[len] != '"') {
    len++;
  }
  add_token(tokens, input, len);

  /* Skip the closing quote if there is one (an unterminated string just
   * runs to the end of the input). */
  return input[len] == '"' ? len + 1 : len;
}

/** Read a word starting at input. Adds it as a token and returns its length. */
static size_t read_word(vect_t *tokens, const char *input) {
  size_t len = 0;
  while (!ends_word(input[len])) {
    len++;
  }
  add_token(tokens, input, len);
  return len;
}

/** Split the input string into a vector of tokens. The caller owns the
 *  returned vector and must free it with vect_delete. */
vect_t *tokenize(const char *input) {
  assert(input != NULL);

  vect_t *tokens = vect_new();
  size_t i = 0;

  while (input[i] != '\0') {
    if (is_whitespace(input[i])) {
      i++;                                   /* skip separators */
    } else if (is_special(input[i])) {
      add_token(tokens, &input[i], 1);       /* one-char token */
      i++;
    } else if (input[i] == '"') {
      i++;                                   /* skip opening quote */
      i += read_quoted(tokens, &input[i]);
    } else {
      i += read_word(tokens, &input[i]);
    }
  }

  return tokens;
}