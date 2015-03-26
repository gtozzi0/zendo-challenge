#ifndef PROCON_H
#define PROCON_H

#include "stack.h"
#include "my_bool.h"

void  init_stack_queue(stack_t*);
void* consumerTask(void* );
void* producerTask(void* );

#endif
