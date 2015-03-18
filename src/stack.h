#include "packet.h"

typedef struct
{
  packet_t      packet;
  stack_node_t *next;
} stack_node_t;

