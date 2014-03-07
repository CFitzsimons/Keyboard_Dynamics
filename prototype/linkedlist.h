#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdlib.h>

struct data{
  void* ptr;
  struct data* next;
}typedef node;

struct{
  node* head;
  node* tail;
  int size;
}typedef linkedlist;

int addFirst(linkedlist*, node*);
int append(linkedlist*, node*);

#endif
