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
  init_stack_queue();

  /* Create some threads */
  pthread_t producer_thread_0;
  pthread_t consumer_thread_0;

  pthread_create(&producer_thread_0, NULL, &producerTask, NULL); 
  usleep(3000000);    // delay 3 sec before consumer starts
  pthread_create(&consumer_thread_0, NULL, &consumerTask, NULL);

  while(1);

  return 0;
}
