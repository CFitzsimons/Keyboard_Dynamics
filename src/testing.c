#include "linkedlist.h"
#include <stdio.h>

int main(){
  linkedlist* ll = malloc(sizeof(linkedlist));
  int num1 = 1;
  int num2 = 2;
  append(ll, &num1);
  append(ll, &num2);
  removeFirst(ll);
  removeFirst(ll);
  removeFirst(ll);
  printf("Element is: %d\n", ll->size); 
}
