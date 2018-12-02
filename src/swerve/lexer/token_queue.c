#include <swerve/lexer/token_queue.h>


TokenQueue* make_token_queue() {
  TokenQueue* queue = malloc(sizeof(TokenQueue));
  queue->head = malloc(sizeof(TokenQueueNode));
  queue->head->read_offset = 0;
  queue->head->write_offset = 0;
  queue->head->next = NULL;
  queue->read_head = queue->head;
  queue->write_tail = queue->head;
  return queue;
}

void free_token_queue(TokenQueue* queue) {
  TokenQueueNode* node = queue->head;
  while (node) {
    TokenQueueNode* current = node;
    node = current->next;
    free(current);
  }
  free(queue);
}

void token_queue_write(TokenQueue* queue, Token token) {
  if (queue->write_tail->write_offset == TOKEN_QUEUE_NODE_SIZE) {
    TokenQueueNode* next = malloc(sizeof(TokenQueueNode));
    queue->write_tail->next = next;
    queue->write_tail = next;
  }
  TokenQueueNode* tail = queue->write_tail;
  tail->slots[tail->write_offset++] = token;
}

Token* token_queue_read(TokenQueue* queue) {
  if (queue->read_head->read_offset == TOKEN_QUEUE_NODE_SIZE) {
    queue->read_head = queue->read_head->next;
  }
  TokenQueueNode* head = queue->read_head;
  if (head && head->read_offset < head->write_offset) {
    return &head->slots[head->read_offset++];
  } else {
    return NULL;
  }
}