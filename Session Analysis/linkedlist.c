#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>

int addFirst(linkedlist* ll, void* ptr){
  node* add = malloc(sizeof(node));
  add->ptr = ptr;
  add->next = NULL;
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

int removeFirst(linkedlist* ll){
  if(ll->size == 0)
    return 0;
  void* freeMe = ll->head;
  if(ll->size == 1){
    ll->head = NULL;
    ll-> tail = NULL;
  }else
    ll->head = ll->head->next;
  free(freeMe);
  ll->size -= 1;
  return 1;
}

int append(linkedlist* ll, void* ptr){
  if(ll->size == 0)
    return addFirst(ll, ptr);
  node* add = malloc(sizeof(node));
  add->ptr = ptr;
  ll->tail->next = add;
  ll->tail = add;
  ll->size += 1;
  return 1;
}

int clear(linkedlist* clear){
  if(clear->size <= 0)
    return 0;
  while(clear->size > 0){
    void* freeMe = clear->head;
    clear->head = clear->head->next;
    free(freeMe);
    clear->size -= 1;
  }
  clear->head = NULL;
  clear->tail = NULL;
  return 1;
}
/*  Sample of Linkedlist working
* linkedlist* ll = malloc(sizeof(linkedlist));
* node toAdd;
* //make sure the data is defined somewhere
* int data = 43;
* //Store the pointer to the data into the node
* toAdd.ptr = &data;
* //Add the node to the list
* append(ll, &toAdd);
*/
