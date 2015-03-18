#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <time.h>

#define ZCH_MAXTAGSIZE  16

typedef int bool;

enum
{
  false = 0,
  true
};

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
  propMax = -2,
  prioAny = -3
};

#endif
