#ifndef STACK_H
#define STACK_H

#include "packet.h"

//TODO: why cant i use the typedef in the structure
typedef struct stack_node
{
  packet_t           packet;
  struct stack_node *next;
} stack_node_t;

/*
typedef struct
{
  stack_node_t* stack;
  semaphore     full_count;
  mutex         stack_mutex;
}
*/

#endif
