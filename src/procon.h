#ifndef PROCON_H
#define PROCON_H

#include "stack.h"

void  init_stack_queue(void);
void* consumerTask(void* );
void* producerTask(void* );
bool  consumerGet(stack_node_t **, packet_t *, int);

#endif
