#include <stdio.h>
#include "queue.h"

// Print out the index and the value of each element.
bool show_one(queue_element* elem, queue_function_args* args) {
  printf("Item %d == %d\n", *(int*) args, *(int*) elem);
  *(int*) args = *(int*) args + 1;
  return true;
}

int main(int argc, char* argv[]) {
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

  return 0;
}
