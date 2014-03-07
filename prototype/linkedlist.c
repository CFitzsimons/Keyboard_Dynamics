#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>

int addFirst(linkedlist* ll, node* add){
  if(ll->size == 0){
    ll->head = add;
    ll->tail = add;
    ll->size += 1;
    return 1;
  }
  add->next = ll->head;
  ll->head = add;
  ll->size += 1;
  return 1;
}

int append(linkedlist* ll, node* add){
  if(ll->size == 0)
    return addFirst(ll, add);
  ll->tail->next = add;
  ll->tail = add;
  ll->size += 1;
  return 1;
}
