#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <time.h>
#include <semaphore.h>
#include "my_bool.h"

#define ZCH_MAXTAGSIZE  16

typedef struct
{
  uint32_t id;
  char     tag[ZCH_MAXTAGSIZE];
  uint8_t  priority;
  uint16_t size;
  long     timestamp;
  uint8_t *data;
} packet_t;

typedef struct stack_node
{
  packet_t           packet;
  struct stack_node* next;
} stack_node_t;

typedef struct
{
  pthread_mutex_t    queue_mutex;
  sem_t              queue_cnt;
  struct stack_node* head;
} stack_t;

enum
{
  prioMin = -1,
  prioMax = -2,
  prioAny = -3
};

/* Prototypes */
bool stack_queue(stack_node_t **, packet_t *);
bool stack_pull_head(stack_node_t **, packet_t *);
bool stack_pull_tail(stack_node_t **, packet_t *);
bool stack_priority_pull(stack_node_t **, packet_t *, uint8_t);

#endif
