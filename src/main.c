#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include "procon.h"

/* NOTE: We don't wait for threads to finish in main because of while(1). */

int main(void)
{
  /* Init stack */
  stack_t stack_0;
  init_stack_queue(&stack_0);

  /* Create some threads */
  pthread_t producer_thread_0;
  pthread_t producer_thread_1;
  pthread_t producer_thread_2;
  //pthread_t producer_thread_3;
  //pthread_t producer_thread_4;

  pthread_t consumer_thread_0;
  pthread_t consumer_thread_1;
  pthread_t consumer_thread_2;
  //pthread_t consumer_thread_3;
  //pthread_t consumer_thread_4;

  /* Pass in the stack_t structure to the threads */
  pthread_create(&producer_thread_0, NULL, &producerTask, &stack_0); 
  pthread_create(&producer_thread_1, NULL, &producerTask, &stack_0); 
  pthread_create(&producer_thread_2, NULL, &producerTask, &stack_0); 
  //pthread_create(&producer_thread_3, NULL, &producerTask, &stack_0); 
  //pthread_create(&producer_thread_4, NULL, &producerTask, &stack_0); 
  
  usleep(2000000);    // delay before consumer starts
  pthread_create(&consumer_thread_0, NULL, &consumerTask, &stack_0);
  pthread_create(&consumer_thread_1, NULL, &consumerTask, &stack_0);
  pthread_create(&consumer_thread_2, NULL, &consumerTask, &stack_0);
  //pthread_create(&consumer_thread_3, NULL, &consumerTask, &stack_0);
  //pthread_create(&consumer_thread_4, NULL, &consumerTask, &stack_0);

  while(1);

  return 0;
}

