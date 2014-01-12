#include <stdio.h>
#include "queue.h"
#include <assert.h>

// Print out the index and the value of each element.
bool show_one(queue_element* elem, queue_function_args* args) {
  printf("Item %d == %d\n", *(int*) args, *(int*) elem);
  *(int*) args = *(int*) args + 1;
  return true;
}

int append_size_test(){
  queue *q = queue_create();
  int x = 0, y = 1, z = 2;
  void *ptr;
  queue_append(q, &x);
  queue_append(q, &y);
  queue_append(q, &z);
  assert(queue_size(q) == 3);
  queue_remove(q, &ptr);
  queue_remove(q, &ptr);
  queue_remove(q, &ptr);
  free(q);
  return 0;
}

int remove_size_test(){
  queue *q = queue_create();
  int x = 0, y = 1, z = 2;
  queue_append(q, &x);
  queue_append(q, &y);
  queue_append(q, &z);
  assert(queue_size(q) == 3);
  queue_element *ret_val;
  queue_remove(q, &ret_val);
  assert(queue_size(q) == 2); 
  free(q);
  return 0;
}

int remove_value_test(){
  queue *q = queue_create();
  int x = 0, y = 1, z = 2;
  queue_append(q, &x);
  queue_append(q, &y);
  queue_append(q, &z);
  assert(queue_size(q) == 3);
  int *ret_val;
  queue_remove(q, (queue_element **)&ret_val); 
  assert(*ret_val == x);
  queue_remove(q, (queue_element **)&ret_val); 
  assert(*ret_val == y);
  queue_remove(q, (queue_element **)&ret_val); 
  assert(*ret_val == z);
  free(q);
  return 0;
}

int append_apply_test(){
  queue* q = queue_create();

  int x = 0;
  int y = 1;
  int z = 2;
  queue_append(q, &x);
  queue_append(q, &y);
  queue_append(q, &z);
  queue_append(q, &x);
  printf("Queue size is %zu\n", queue_size(q));
  
  int index = 0;
  queue_apply(q, show_one, &index);
  free(q);
  return 0;
}

int reverse_test(){
  queue* q = queue_create();

  int x = 0;
  int y = 1;
  int z = 2;
  queue_append(q, &x);
  queue_append(q, &y);
  queue_append(q, &z);
  
  assert(queue_size(q) == 3);
  int *ret_val;
  queue_reverse(q);
  queue_remove(q, (queue_element **)&ret_val); 
  assert(*ret_val == z);
  queue_remove(q, (queue_element **)&ret_val); 
  assert(*ret_val == y);
  queue_remove(q, (queue_element **)&ret_val); 
  assert(*ret_val == x);
  free(q);
  return 0;
}

static int queue_comp(queue_element *e1, queue_element *e2){
  int res = *(int *)e1 - *(int *)e2; 
  if(res < 0) return -1;
  if(res == 0) return 0;
  return 1;
}

int sort_test(){
  queue* q = queue_create();
  int w = 506;
  int x = -5466;
  int y = 90000;
  int z = 0;
  queue_append(q, &w);
  queue_append(q, &z);
  queue_append(q, &x);
  queue_append(q, &y);
  queue_append(q, &z);
  assert(queue_size(q) == 5);
  int *ret_val;
  queue_sort(q, &queue_comp);
  queue_remove(q, (queue_element **)&ret_val); 
  int prev = *ret_val;
  while(!queue_is_empty(q)){
    queue_remove(q, (queue_element **)&ret_val); 
    assert(prev <= *ret_val);
    prev = *ret_val;
  }
  free(q);
  return 0;
}
int main(int argc, char* argv[]) {
  int failct = 0;
  failct += append_size_test();
  failct += append_apply_test();
  failct += remove_size_test();
  failct += remove_value_test();
  failct += reverse_test();
  failct += sort_test();
  if(failct == 0){
    printf("All tests successful.\n");
  }else{
    printf("Some tests did not pass successfully.\n");
  }
}
