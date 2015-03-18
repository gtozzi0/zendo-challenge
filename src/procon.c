#include <stdint.h>
#include <stdlib.h>
#include "packet.h"

//TODO: another approach would be to buffer incoming data from producers at the
//stack layer. The stack thread would handle atomically sorting out data
//periodically. This way, mutexs would only need be handled by the stack. The
//consumer thread would still need to use mutex though...

/* This mutex needs to be accessible from all of the producer and consumer
 * threads. 
 */

/* I guess this function takes stack and packet from the calling thread */
int producerPut(stack_t *stack, packet_t *packet)
{
  

}


/* Gets id from revolving 32-bit counter */
// TODO: this may need to be atomic
static uint32_t get_id(void)
{
  static uint32_t id = 0;

  return (id++);
}


void producerTask(void *)
{
  while(1)
  { 
    //delay rand time between 0 and 1 sec

    packet_t packet;

    packet.id   = get_id(); 
    //packet.tag = ?
    packet.prio = (uint8_t ) rand();
    packet.size = 0x0000003F & (uint32_t )rand();
    packet.data = malloc(sizeof(uint8_t) * packet.size);
    
    /* Grab mutex? */
    // this will copy the data from packet into the stack somehow
    // it should signal to the stack, maybe thats what &stack is for
    producerPut(&stack, &packet);
    /* Release mutex? */
  }
}


