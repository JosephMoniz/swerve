#pragma once

#include <swerve/lexer/token.h>

#define TOKEN_QUEUE_NODE_SIZE (((64 * 1024) / sizeof(Token)) - 1)

typedef struct _TokenQueueNode {
  Token slots[TOKEN_QUEUE_NODE_SIZE];
  size_t read_offset;
  size_t write_offset;
  struct _TokenQueueNode* next;
} TokenQueueNode;

typedef struct _TokenQueue {
  TokenQueueNode *head;
  TokenQueueNode* read_head;
  TokenQueueNode* write_tail;
} TokenQueue;

TokenQueue* make_token_queue();

void free_token_queue(TokenQueue* queue);

void token_queue_write(TokenQueue* queue, Token token);

Token* token_queue_read(TokenQueue* queue);