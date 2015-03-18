
#define bool            int
#define ZCH_MAXTAGSIZE  16

typedef enum
{
  false = 0,
  true
} bool;

typedef struct
{
  uint32_t id;
  char     tag[ZCH_MAX_TAGSIZE];
  uint8_t  prio;
  uint16_t size;
  uint8_t *data;
} packet_t;

enum
{
  prioMin = -1,
  propMax = -2,
  prioAny = -3
};


typedef struct
{
  // thread arguments?
  // time variable?
  
} producer_t;

