#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/time.h>
#include "stack.h"
#include "basic_time.h"
#include "procon.h"
#include "my_bool.h"

/*** Private Functions ***/
static bool     producerPut(stack_node_t**, packet_t*);
static bool     consumerGet(stack_node_t**, packet_t*, int);
static void     fill_packet(packet_t*);
static void     process_packet(packet_t*);
static uint32_t get_id(void);
static int      get_rand_stack_prio(void);

/*** Global Variables ***/
static pthread_mutex_t rand_mutex = PTHREAD_MUTEX_INITIALIZER;


/* Stack Init. The stack automatically detects a NULL head and creates a new
 * stack from it. We only init the mutex and semaphore here.
 */
void init_stack_queue(stack_t* stack)
{
  /* Init the semaphore. Stack head is auto-init. */
  sem_init(&stack->queue_cnt, 0, 0);
  /* Initialize mutex with default attributes */
  pthread_mutex_init(&stack->queue_mutex, NULL);
}

/* Consumer Thread */
void* consumerTask(void* stack)
{
  stack_t*       stack_ptr = (stack_t* )stack; 
  packet_t       packet;
  int            priority;
  unsigned int   usecs;
  struct timeval tv;

  /* Seed rand() with time */
  pthread_mutex_lock(&rand_mutex);
  gettimeofday(&tv, NULL);
  srand((int )tv.tv_usec);
  pthread_mutex_unlock(&rand_mutex);

  while(1)
  {
    /* usleep takes unsigned int. choose random time between 0 and ~1sec */
    /* The time could be longer since getting the rand time depends on mutex */
    pthread_mutex_lock(&rand_mutex);
    usecs = (unsigned int )(rand() & 0x000FFFFF);
    pthread_mutex_unlock(&rand_mutex);

    usleep(usecs);

    /* Block and wait for queue entry. If the cnt is at 0, we wait cause it's
     * empty. Once it goes positive, decrement the cnt and process. We don't
     * have anything better to do, so just spin on the semaphore.
     */
    sem_wait(&stack_ptr->queue_cnt);

    /* Get a random priority from values -3,-2,-1,0-255 */
    pthread_mutex_lock(&rand_mutex);
    priority = get_rand_stack_prio();
    pthread_mutex_unlock(&rand_mutex);

    /* Lock down the stack */
    pthread_mutex_lock(&stack_ptr->queue_mutex);
  
#ifdef CON_DEBUG
    printf("Consumer Thread is consuming!\n");
#endif

    if (!consumerGet(&stack_ptr->head, &packet, priority))
      printf("Consumer thread X failed to get a packet!\n");

    /* Unlock the stack */
    pthread_mutex_unlock(&stack_ptr->queue_mutex);

    process_packet(&packet);
  }
  return NULL;
}

/* Producer Thread */
void* producerTask(void* stack)
{
  stack_t*       stack_ptr = (stack_t* )stack;
  packet_t       packet;
  pthread_t      self_id;
  unsigned int   usecs;
  struct timeval tv;
  
  /* Assign the thread ID as the packet tag */
  /* This isn't portable or even necessarily proper because thread_t is an
   * opaque type. We're also not checking that the converted pthread string id
   * is small enough for the tag array. But this will suffice as some value for
   * the string tag for now. Also, the sprintf throws a warning.
   */
  self_id = pthread_self();
  sprintf(packet.tag, "%d", (int )self_id);  

  /* Seed rand() with time in usec resolution. */
  pthread_mutex_lock(&rand_mutex);
  gettimeofday(&tv, NULL);
  srand((int )tv.tv_usec);
  pthread_mutex_unlock(&rand_mutex);

  while (1)
  {
    /* usleep takes  unsigned int. choose random time between 0 and ~1sec */
    pthread_mutex_lock(&rand_mutex);
    usecs = (unsigned int )(rand() & 0x000FFFFF);
    pthread_mutex_unlock(&rand_mutex);

    usleep(usecs);

#ifdef CON_DEBUG
    printf("Producer Thread is producing!\n");
#endif

    /* Atomically fill a packet. Namely to atomically grab from the ID counter
     * and atomically use rand() 
     */
    pthread_mutex_lock(&rand_mutex);
    fill_packet(&packet);
    pthread_mutex_unlock(&rand_mutex);

    /* Lock down the stack */
    pthread_mutex_lock(&stack_ptr->queue_mutex);

    if (!producerPut(&stack_ptr->head, &packet))
      printf("Producer thread X failed to queue a message!\n");
  
/*
TODO
    if(!stack_queue(&stack_ptr->head, &packet))
      printf("Producer thread X failed to queue a message!\n");
*/

    /* Increment the semaphore indicating another item in queue */ 
    sem_post(&stack_ptr->queue_cnt);

    /* Unlock the stack */
    pthread_mutex_unlock(&stack_ptr->queue_mutex);
  }
}

static bool consumerGet(stack_node_t **head, packet_t *packet, int priority)
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

/* Wrapper function for stack queue */
static bool producerPut(stack_node_t **head, packet_t *packet)
{
  if (!stack_queue(head, packet))
    return false;

  return true;
}

/* NOTE: Not a reentrant function!
 * Gets id from revolving 32-bit counter. This function needs to be called
 * inside a thread mutex to keep the counter atomic. It would be better form
 * to make this reentrant, but for now, offload that concern to calling
 * function. 
 */
static uint32_t get_id(void)
{
  static uint32_t id = 0;
  
  return (id++);
}

static void fill_packet(packet_t* packet)
{
  int      i;
  uint8_t* data_ptr;

  packet->priority  = (uint8_t )rand();
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

static void process_packet(packet_t* packet)
{
  int i;
  uint8_t* data_ptr = NULL;

  printf("Packet tag      : %s\n", packet->tag);
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

