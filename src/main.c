#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "procon.h"

//TODO: another approach would be to buffer incoming data from producers at the
//stack layer. The stack thread would handle atomically sorting out data
//periodically. This way, mutexs would only need be handled by the stack. The
//consumer thread would still need to use mutex though...

int main(void)
{
  /* Init stack */
  stack_t stack_0;
  init_stack_queue(&stack_0);

  /* Create some threads */
  pthread_t producer_thread_0;
  pthread_t consumer_thread_0;

  /* Pass in the stack_t structure to the threads */
  pthread_create(&producer_thread_0, NULL, &producerTask, &stack_0); 
  usleep(5000000);    // delay before consumer starts
  pthread_create(&consumer_thread_0, NULL, &consumerTask, &stack_0);

  while(1);

  return 0;
}

