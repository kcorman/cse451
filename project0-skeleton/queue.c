/* Implements queue abstract data type. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/* Each link in the queue stores a queue_element and
 * a pointer to the next link in the queue. */
typedef struct _queue_link {
  queue_element* elem;
  struct _queue_link* next;
} queue_link;

/* This is the actual implementation of the queue struct that
 * is declared in queue.h. */
struct _queue {
  queue_link* head;
};

queue* queue_create() {
  queue* q = (queue*) malloc(sizeof(queue));

  q->head = NULL;
  return q;
}

/* Private */
static queue_link* queue_new_element(queue_element* elem) {
  queue_link* ql = (queue_link*) malloc(sizeof(queue_link));

  ql->elem = elem;
  ql->next = NULL;

  return ql;
}

void queue_append(queue* q, queue_element* elem) {
   queue_link* cur;
   assert(q != NULL);
  //handle empty queue case
  if(!q->head){
    q->head = queue_new_element(elem);
  }else{
    // Find the last link in the queue.
    for (cur = q->head; cur->next; cur = cur->next) {}

    // Append the new link.
    cur->next = queue_new_element(elem);
  }
}

bool queue_remove(queue* q, queue_element** elem_ptr) {
  queue_link* old_head;

  assert(q != NULL);
  assert(elem_ptr != NULL);
  if (queue_is_empty(q)) {
    return false;
  }

  *elem_ptr = q->head->elem;
  old_head = q->head;
  q->head = q->head->next;
  free(old_head);
  return true;
}

bool queue_is_empty(queue* q) {
  assert(q != NULL);
  return q->head == NULL;
}

/* See queue.h for documentation */
void queue_reverse(queue* q){
  queue_link *current, *prev, *next;
  current = q->head;
  prev = NULL;
  while(current != NULL){
    //set current to point to the previous node,
    //but first store current->next
    next = current->next;
    current->next = prev;
    //update previous to be our current node now that we used it
    prev = current;
    current = next;
  }
  q->head = prev;
}

/* private */
static bool queue_count_one(queue_element* elem, queue_function_args* args) {
  size_t* count = (size_t*) args;
  *count = *count + 1;
  return true;
}

size_t queue_size(queue* q) {
  size_t count = 0;
  queue_apply(q, queue_count_one, &count);
  return count;
}

bool queue_apply(queue* q, queue_function qf, queue_function_args* args) {
  assert(q != NULL && qf != NULL);

  if (queue_is_empty(q))
    return false;

  for (queue_link* cur = q->head; cur; cur = cur->next) {
    if (!qf(cur->elem, args))
      break;
  }

  return true;
}
static void swap(queue_link *bef_a, queue_link *bef_b){
  queue_link *temp = bef_a->next;
  bef_a->next = bef_b->next;
  bef_b->next = temp;
  //swap stuff that swapped nodes point to
  temp = bef_a->next->next;
  bef_a->next->next = bef_b->next->next;
  bef_b->next->next = temp;
}

void queue_sort(queue* q, queue_compare qc){
  //simple selection sort
  //current is rightmost sorted element
  queue_link head_link;
  head_link.next = q->head;
  queue_link *current = &head_link;
  while(current && current->next != NULL){
    //pick the smallest next element after current
    //to be current->next, preserving the rest of the list structure
    queue_link *bef_smallest = current;
    queue_link *itr = current;
    while(itr->next != NULL){
      if(qc(bef_smallest->next->elem, itr->next->elem) > 0){
        bef_smallest = itr;
      }
      itr = itr->next;
    }
    //now we have a ptr to the element right before the smallest one
    //so swap out the smallest with current->next
    //queue_element *elem = current->next->elem;
    //current->next->elem = bef_smallest->next->elem;
    //bef_smallest->elem = elem;
    swap(current, bef_smallest);
    //update current
    current = current->next;
  }
  q->head = head_link.next;
}

void queue_destroy(queue *q, bool free_elems){
  queue_link *next = NULL; 
  for (queue_link* cur = q->head; cur; cur = next) {
    next = cur->next;
    if(free_elems){
      free(cur->elem);
    }
    free(cur);
  }
  free(q);
}
