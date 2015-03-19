#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <time.h>

#define ZCH_MAXTAGSIZE  16

typedef int bool;

enum
{
  false = 0,
  true
};

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

typedef struct
{
  uint32_t id;
  char     tag[ZCH_MAXTAGSIZE];
  uint8_t  priority;
  uint16_t size;
  long     timestamp;
  uint8_t *data;
} packet_t;

enum
{
  prioMin = -1,
  prioMax = -2,
  prioAny = -3
};

#endif
