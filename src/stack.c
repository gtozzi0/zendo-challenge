#include <stdint.h>
#include "stack.h"
#include "packet.h"

//TODO: maybe define bool and enum true false in seperate header
//TODO: Dequeueing the lowest priority could be faster with doubly linked list

//TODO: we never set a max size limit for the stack or check. We could with
//count

/* Returns a head pointer. Caller must manage the head ptr to a stack. */
stack_node_t* stack_init(void)
{
  stack_node_t* head;

  head = (stack_node_t * )malloc(sizeof(stack_node_t));

  head->next = NULL;

  return head;
}

/* Iterates through the stack and places the packet in the appropriate spot
 * according to 1) priority 2) timestamp. Returns false if head pointer is
 * invalid. Returns true on successful insertion. We keep track of the current
 * node and previous node for insertion before and after current. The head
 * address may need to be modified in here, so we require a pointer to the head
 * pointer.
 */
//TODO: note the const new packet since we do not want to edit the packet data
bool stack_queue(stack_node_t **head, packet_t const *new_packet)
{
  stack_node_t* new;
  stack_node_t* previous;
  stack_node_t* current;
  int           count = 0;

  if (*head == NULL) 
    return false;
  
  new = (stack_node_t * )malloc(sizeof(stack_node_t));

  /* Use copy of head pointer to iterate through list */
  current = *head;

  while (current != NULL)
  {
    /* Check priority of current node packet */
    if (new_packet->priority > current->packet.priority)
    {
      /* Check if at head position */
      if (count == 0)
      {
         new->next = current;
        *head      = new;       // We must update our head
         break;
      }

      previous->next = new;
      new->next      = current;
      
      //TODO: notes on this below
      memcpy(&new->packet, new_packet, sizeof(packet_t); 
      break;
    }
    else if (current->packet.priority == new_packet->priority)
    {
      /* Compare timestamps, earlier has priority */
      if (new_packet->timestamp < current->packet.priority)
      {
        /* Check if at head position */
        if (count == 0)
        {
           new->next = current;
          *head      = new;     // We must update our head
           break;
        }

        previous->next = new;
        new->next      = current;
        
        memcpy(&new->packet, new_packet, sizeof(packet_t); 
        break;
      }
    }
  
    /* Check if we are at tail. If so, insert the new node here */
    if (current->next == NULL)
    {
      prev->next = new;
      new->next  = NULL;
      /* We found insertion point, break out of loop */
      break;
    }
    
     /* Store previous node in case insertion before current */
     prev_node = current;
     current   = current->next; 
     ++count;
  }
  
  return true;
}

bool stack_pull_head(stack_node_t **head, packet_t *packet)
{
  stack_node_t* current;

  if (*head == NULL) 
    return false;

  current = *head;
  //TODO: how do i pass the address of the head packet?
  //memcpy(packet, &(*head)->packet, sizeof(packet_t)); 

  memcpy(packet, &current->packet, sizeof(packet_t)); 
    
  *head = current->next;
  // or *head = (*head)->current;

  free(current);
  return true;
}

bool stack_pull_tail(stack_node_t *current, packet_t *packet)
{
  stack_node_t* previous;
  int           count = 0;

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

  /* Must preserve the head */
  if (count == 0)
  {

  }

  previous->next = NULL;

  free(current);
  return true;
}

bool stack_priority_pull(stack_node_t *current, packet_t *packet, uint8_t prio)
{

}


/* Iterate through the list looking for the correct priority or the priority
 * closest to the requested. If it's the highest priority, pull from the head.
 * If it's lowest priority, iterate to NULL and pull that. The queue method has
 * already sorted out the priorities (including timestamp). 
 */ 
// We know int is larger than uint8_t on any system with the exception of
// possibly an 8-bit microcontroller...
bool stack_dequeue(stack_node_t **head, packet_t *packet, int priority)
{
  stack_node_t* previous;
  stack_node_t* current;
  int           count = 0;
  uint8_t       prio;  

  if (*head == NULL) 
    return false;

  if (priority < PrioAny || priority > 255)
    return false;

  /* Use copy of head pointer to iterate through list */
  current = *head;

  if (priority == prioAny || priority == prioMax)
  {
    //pull the head
    
    
  }
  else if (priority >= 0)
  {

  }
  else  //prioMin
  {
    while (current->next != NULL) 
    {
      
    }
  }


  while (current != NULL)
  {
    if (priority == prioMax || priority == prioAny)
    {

    }
    else if (priority == prioMin)
    {
      while ( 
    
  }
   
}





