#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <pthread.h>
#include <semaphore.h>


static pthread_mutex_t stack_mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t           stack_queue_cnt;

void init_stack_queue(void)
{
  //initialize queue
  
  sem_init(&stack_queue_cnt, 0 , 0);
}

/* Consumer Thread */
void* consumerTask(void* arg)
{
  while(1)
  {
    /* Block and wait for queue entry. If the cnt is at 0, we wait cause it's
     * empty. Once it goes positive, decrement the cnt and process.
     */
    sem_wait(&stack_queue_cnt);
  
    /* Lock down the stack */
    pthread_mutex_lock(&stack_mutex);

    //TODO: pop off a message
    
    /* Unlock the stack */
    pthread_mutex_unlock(&stack_mutex);

    //TODO: process the job
    //TODO: free() to clean up?? 
  }
  return NULL;
}


void* producerTask(void* arg)
{
  while (1)
  {
    //create a message

    /* Lock down the stack */
    pthread_mutex_lock(&stack_mutex);
    
    //TODO: queue the message into the stack

    /* Increment the semaphore indicating another item in queue */ 
    sem_post(&stack_queue_cnt);

    /* Unlock the stack */
    pthread_mutex_unlock(&stack_mutex);
  }
}

