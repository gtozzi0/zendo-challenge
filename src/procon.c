#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "stack.h"
#include "basic_time.h"
#include "procon.h"
#include "my_bool.h"

//TODO: seed rand with time

/* Global variables */
static stack_node_t *stack         = NULL;
static pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t           stack_queue_cnt;

/* Private Functions */
static void     fill_packet(packet_t*);
static void     process_packet(packet_t*);
static uint32_t get_id(void);
static int      get_rand_stack_prio(void);

//TODO TODO TODO
//Does each producer get its own counter? Or do they all pull from the same
//counter? If they each get their own, I think we can put static id variable
//in the producerTask, and that gives each instance its own id counter

void init_stack_queue(void)
{
  /* Init the semaphore. Stack head is auto-init. */
  sem_init(&stack_queue_cnt, 0 , 0);
}

static void fill_packet(packet_t* packet)
{
  int      i;
  uint8_t *data_ptr;

  //TODO: packet.tag = ?
  packet->priority  = (uint8_t ) rand();
  packet->size      = 0x0000003F & (uint32_t )rand();
  packet->data      = malloc(sizeof(uint8_t) * packet->size);
  packet->timestamp = get_ms_timestamp();
  packet->id        = get_id(); 

  data_ptr = packet->data;

  for (i = 0; i < packet->size; i++)
  {
    *data_ptr = (uint8_t )rand();
    ++data_ptr;
  }
}

static int get_rand_stack_prio(void)
{
  int priority;

  priority = (uint32_t )(rand() & 0x00000003);

  switch (priority)
  {
    case 0:
      priority = (uint8_t )rand(); 
      break;
    case 1:
      priority = prioAny;
      break;
    case 2:
      priority = prioMax;
      break;
    case 3:
      priority = prioMin;
      break;
    default:
      priority = prioAny;
      break;
  }
  return (priority);
}

/* Consumer Thread */
void* consumerTask(void* unused)
{
  packet_t     packet;
  int          priority;
  unsigned int usecs;

  while(1)
  {
    /* usleep takes  unsigned int. choose random time between 0 and ~1sec */
    usecs = (unsigned int )(rand() & 0x000FFFFF);
    usleep(usecs);

    /* Get a random priority from values -3,-2,-1,0-255 */
    priority = get_rand_stack_prio();

    /* Block and wait for queue entry. If the cnt is at 0, we wait cause it's
     * empty. Once it goes positive, decrement the cnt and process.
     */
    sem_wait(&stack_queue_cnt);
  
#ifdef CON_DEBUG
    printf("Consumer Thread is consuming!\n");
#endif

    /* Lock down the stack */
    pthread_mutex_lock(&stack_mutex);

    if (!consumerGet(&stack, &packet, priority))
      printf("Consumer thread X failed to get a packet!\n");

    /* Unlock the stack */
    pthread_mutex_unlock(&stack_mutex);

    process_packet(&packet);
  }
  return NULL;
}

static void process_packet(packet_t *packet)
{
  int i;
  uint8_t *data_ptr = NULL;

  //TODO: print tag that tells which thread it came from
  printf("Packet id       : %u\n", packet->id);
  printf("Packet priority : %u\n", packet->priority);
  printf("Packet timestamp: %ld\n", packet->timestamp);
  printf("Packet size     : %u\n", packet->size);
  printf("Packet data     : *\n");
  
  /* Get copy of data ptr so we keep address for freeing */
  data_ptr = packet->data;

  for (i = 0; i < packet->size; i++)
  {
    printf("%02X ", *data_ptr); 
    ++data_ptr;

    if ((i+1)%8 == 0)
      printf("\n");
  }
  printf("\n\n");

  /* Free the packet data memory */
  free(packet->data);
}

/* Producer Thread */
void* producerTask(void* unused)
{
  packet_t packet;
  unsigned int usecs;

  while (1)
  {
    /* usleep takes  unsigned int. choose random time between 0 and ~1sec */
    usecs = (unsigned int )(rand() & 0x000FFFFF);
    usleep(usecs);

#ifdef CON_DEBUG
    printf("Producer Thread is producing!\n");
#endif

    /* Lock down the stack */
    pthread_mutex_lock(&stack_mutex);
    
    /* Atomically fill a packet. Namely to atomically grab from the ID counter */
    fill_packet(&packet);

    if(!stack_queue(&stack, &packet))
      printf("Producer thread X failed to queue a message!\n");

    /* Increment the semaphore indicating another item in queue */ 
    sem_post(&stack_queue_cnt);

    /* Unlock the stack */
    pthread_mutex_unlock(&stack_mutex);
  }
}

bool consumerGet(stack_node_t **head, packet_t *packet, int priority)
{
  if (*head == NULL) 
  {
#ifdef CON_DEBUG
    printf("HEAD WAS EQUAL TO NULL\n");
#endif
    return false;
  }

  /* Check for valid priority */
  if (priority < prioAny || priority > 255)
  {
#ifdef CON_DEBUG
    printf("INVALID PRIORITY\n");
#endif
    return false;
  }

  if (priority == prioAny || priority == prioMax)
  {
    if (!stack_pull_head(head, packet))
    {
#ifdef CON_DEBUG
    printf("BAD HEAD PULL\n");
#endif
      return false;
    }
  }
  else if (priority >= 0)
  {
    if (!stack_priority_pull(head, packet, priority))
    {
#ifdef CON_DEBUG
    printf("BAD PRIO PULL\n");
#endif
      return false;
    }
  }
  else  //prioMin
  {
    if (!stack_pull_tail(head, packet))
    {
#ifdef CON_DEBUG
    printf("BAD TAIL PULL\n");
#endif
      return false;
    }
  }

  return true;
}

/* Gets id from revolving 32-bit counter. This function needs to be called
 * inside the thread mutex to keep the counter atomic. It would be better form
 * to make this reentrant, but for now, offload that concern to calling
 * function. return (id++) takes more than one instruction to
 * execute. 
 */
static uint32_t get_id(void)
{
  static uint32_t id = 0;
  
  return (id++);
}

#ifdef NO_COMPILE
/* I guess this function takes stack and packet from the calling thread */
int producerPut(stack_t *stack, packet_t *packet)
{
  

}
#endif

