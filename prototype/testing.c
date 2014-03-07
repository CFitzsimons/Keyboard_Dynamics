#include "linkedlist.h"

int main(){
  linkedlist* ll = malloc(sizeof(linkedlist));
  node test;
  int hello = 5;
  test.ptr = &hello;
  append(ll, &test);
}
