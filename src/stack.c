#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "stack.h"

#ifdef STACK_TEST
#include <stdio.h>
#include "basic_time.h"
#endif

//TODO: maybe define bool and enum true false in seperate header

/* 
 * Note: Dequeueing the lowest priorirty node could be faster by keeping track
 * of the tail node. 
 */

/* 
 * Note: we never set a max size limit for the stack or check. We could return a
 * false on a queue request based on a count of nodes.
 */

/* Iterates through the stack and places the packet in the appropriate spot
 * according to 1) priority 2) timestamp. Returns false if head pointer is
 * invalid. Returns true on successful insertion. We keep track of the current
 * node and previous node for insertion before and after current. If a stack
 * needs to be initialized. The caller just needs to send a packet_t* pointing
 * to NULL. This queue call will handle the rest.
 */
//TODO: note the const new packet since we do not want to edit the packet data
//bool stack_queue(stack_node_t **head, packet_t const *new_packet)

bool stack_queue(stack_node_t **head, packet_t *new_packet)
{
  stack_node_t* new;
  stack_node_t* previous;
  stack_node_t* current;
  int           count = 0;

  /* Create a new 'stack' if one doesn't exist */
  if (*head == NULL) 
  {
     *head        = (stack_node_t* )malloc(sizeof(stack_node_t));
    (*head)->next = NULL;

    memcpy(&(*head)->packet, new_packet, sizeof(packet_t));

#ifdef STACK_DEBUG
    printf("We are in new stack head area\n");
#endif

    return true;
  }
  
  new = (stack_node_t* )malloc(sizeof(stack_node_t));

  /* Use copy of head pointer to iterate through list */
  current = *head;

  while (current != NULL)
  {
    /* Check priority of current node packet */
    if (new_packet->priority > current->packet.priority)
    {
#ifdef STACK_DEBUG
      printf("We are at higher prio spot\n");
#endif

      //TODO: notes on this below
      memcpy(&new->packet, new_packet, sizeof(packet_t));

      /* Check if at head position */
      if (count == 0)
      {
         new->next = current;
        *head      = new;       // We must update our head
         break;
      }

      previous->next = new;
      new->next      = current;
      
      break;
    }
    else if (current->packet.priority == new_packet->priority)
    {
#ifdef STACK_DEBUG
      printf("We are at equal prio spot\n");
#endif
      /* Compare timestamps, earlier has priority */
      if (new_packet->timestamp < current->packet.priority)
      {
        memcpy(&new->packet, new_packet, sizeof(packet_t));

        /* Check if at head position */
        if (count == 0)
        {
           new->next = current;
          *head      = new;     // We must update our head
           break;
        }

        previous->next = new;
        new->next      = current;
        
        break;
      }
    }
  
    /* Check if we are at tail. If so, insert the new node here */
    if (current->next == NULL)
    {
#ifdef STACK_DEBUG
      printf("We are at tail already spot\n");
#endif

      memcpy(&new->packet, new_packet, sizeof(packet_t));

      current->next = new;
      new->next  = NULL;
      /* We found insertion point, break out of loop */
      break;
    }
    
     /* Store previous node in case insertion before current */
     previous = current;
     current  = current->next; 
     ++count;
  }
  
  return true;
}

bool stack_pull_head(stack_node_t **head, packet_t *packet)
{
  stack_node_t* current;

  current = *head;

  if (current == NULL) 
    return false;

  //if (*head == NULL) 
    //return false;

  //TODO: how do i pass the address of the head packet?
  //memcpy(packet, &(*head)->packet, sizeof(packet_t)); 

  memcpy(packet, &current->packet, sizeof(packet_t)); 
    
  *head = current->next;
  // or *head = (*head)->current;

  free(current);
  return true;
}

bool stack_pull_tail(stack_node_t **head, packet_t *packet)
{
  stack_node_t* current  = NULL;
  stack_node_t* previous = NULL;

  current = *head;

  if (current == NULL)
    return false;

  /* We start at the head, looking for the tail */
  while (current->next != NULL)
  {
    previous = current; 
    current  = current->next; 
  }
  
  /* We found the tail */
  memcpy(packet, &current->packet, sizeof(packet_t));  

  /* If previous is NULL, then we are popping the head */
  if (previous == NULL)
    /* We need to update head pointer to NULL */  
    *head = NULL;
  else
    previous->next = NULL;

  free(current);
  return true;
}

// Might need to pass head ref here...
bool stack_priority_pull(stack_node_t **head, packet_t *packet, uint8_t prio)
{
  stack_node_t* current  = NULL;
  stack_node_t* previous = NULL;

  current = *head;

  if (current == NULL)
    return false;

  /* We start at the head, looking for matching priority */
  while (current != NULL) 
  {
    /* Look for a node priority less than or equal to the priority we were given.
     * Or check to see if we are at the tail (the case where a priority we
     * were given is lower than the priority of the tail node).
     */

      /* We need logic for popping head, tail, and something in between */
    if ( (prio >= current->packet.priority) || (current->next == NULL) )
    {
      memcpy(packet, &current->packet, sizeof(packet_t)); 

      if (previous == NULL)                // we are at head
      {
        if (current->next == NULL)         // and head is also tail
        {
          *head = NULL;
        }
        else                               // there is at least one more node
        {
          *head = current->next;
        }
      }
      else if (current->next == NULL)      // we are at tail
      {
        previous->next = NULL;
      }
      else                                 // pull from between two nodes
      {
        previous->next = current->next;           
      }
        
      free (current);
      break;
    }

    /* Else go to next node */
    previous = current;
    current  = current->next;
  }
 
  return true;
}

#ifdef STACK_TEST
void fill_packet(packet_t *packet)
{
  static uint32_t id = 0;

  packet->id   = id++; 
  //packet.tag = ?
  packet->priority = (uint8_t ) rand();
  packet->size = 0x0000003F & (uint32_t )rand();
  //packet.data = malloc(sizeof(uint8_t) * packet.size);
  packet->timestamp = get_ms_timestamp();
  
}

int main(void)
{
  /* The stack API requires ptr init'ed to NULL */
  static stack_node_t *stackHead = NULL;
  packet_t             queue_packet;
  packet_t             dequeue_packet;
  int                  i;


#ifdef POOPY
  /* Test 1: check first entry head pop */ 
  fill_packet(&queue_packet);

  if (!stack_queue(&stackHead, &queue_packet))
    printf("Failed test1 at queue\n");
  else
  {
    if(!stack_pull_head(&stackHead, &dequeue_packet))
      printf("Failed test1 at head pop\n");
  
    printf("Packet id: %u\n", dequeue_packet.id);
    printf("Packet prio: %u\n", dequeue_packet.priority);
    printf("Packet size: %u\n", dequeue_packet.size);
    printf("Packet timestamp: %ld\n", dequeue_packet.timestamp);
  }
  
  /* Test 2: check first entry tail pop */ 
  fill_packet(&queue_packet);

  if (!stack_queue(&stackHead, &queue_packet))
    printf("Failed test2 at queue\n");
  else
  {
    if(!stack_pull_tail(&stackHead, &dequeue_packet))
      printf("Failed test2 at head pop\n");
  
    printf("Packet id: %u\n", dequeue_packet.id);
    printf("Packet prio: %u\n", dequeue_packet.priority);
    printf("Packet size: %u\n", dequeue_packet.size);
    printf("Packet timestamp: %ld\n", dequeue_packet.timestamp);
  }

  /* Test 3: check first entry priority pop */ 
  fill_packet(&queue_packet);

  if (!stack_queue(&stackHead, &queue_packet))
    printf("Failed test3 at queue\n");
  else
  {
    if (!stack_priority_pull(&stackHead, &dequeue_packet, (uint8_t )rand()) )
      printf("Failed test3 at prio pop\n");
  
    printf("Packet id: %u\n", dequeue_packet.id);
    printf("Packet prio: %u\n", dequeue_packet.priority);
    printf("Packet size: %u\n", dequeue_packet.size);
    printf("Packet timestamp: %ld\n", dequeue_packet.timestamp);
  }
#endif
    
  
  /* Test different priorities at the head.... */

  /* Test 5: Queue a bunch of packets and head pull them all */

  for (i = 0; i < 30; i++)
  {
    fill_packet(&queue_packet);

/*
    printf("***PUSH***\n");
    printf("Packet id: %u\n", queue_packet.id);
    printf("Packet prio: %u\n", queue_packet.priority);
    printf("Packet size: %u\n", queue_packet.size);
    printf("Packet timestamp: %ld\n", queue_packet.timestamp);
    printf("\n\n");
*/

    if (!stack_queue(&stackHead, &queue_packet))
      printf("Failed test1 at queue\n");
  }

  /* This is where we need a semaphore to signal consumer to keep pulling */
  for (i = 0; i < 30; i++)
  {
    if(!stack_priority_pull(&stackHead, &dequeue_packet, (uint8_t )rand()) )
      printf("Failed iteration %d head pop\n", i);
  
    printf("iteration %d\n", i);
    printf("***POP***\n");
    printf("Packet id: %u\n", dequeue_packet.id);
    printf("Packet prio: %u\n", dequeue_packet.priority);
    printf("Packet size: %u\n", dequeue_packet.size);
    printf("Packet timestamp: %ld\n", dequeue_packet.timestamp);
    printf("\n\n");
  }


  return 0;
}
#endif

