struct{
   double time;
   int keycode;
   int isUp;
}typedef keypress;

#include "linkedlist.h"
#include <stdio.h>

int main(){
  FILE* fp;
  linkedlist* ll = malloc(sizeof(linkedlist));
  ll->size = 0;
  fp = fopen("dylan_friday.bin", "rb");
  FILE* lol;
  lol = fopen("friday.txt", "w");
  fflush(fp);
  keypress* tmp = malloc(sizeof(keypress));
  while ( fread(tmp, sizeof(keypress), 1, fp) ){
    
    //printf("%f, %d, %d\n", tmp->time, tmp->keycode, tmp->isUp);
    if(tmp->isUp == 1){
      append(ll, tmp);
    }
    tmp = malloc(sizeof(keypress));
  }
  int array [100];
  int blah [100][100];
  for(int i = 0; i < 100; i++){
    array[i] = 0;
    for(int ii = 0; ii < 100; ii++){
      blah[i][ii] = 0;
    }
  }
  keypress now;
    now = *((keypress*)ll->head->ptr);
    //printf("%f, %d, %d\n", now.time, now.keycode, now.isUp);
    if(now.keycode < 100)
      array[now.keycode] += 1;
    removeFirst(ll);
    
  keypress prev;
  prev = now;  
  while(ll->size > 0){
    
    //keypress put;
    //put = *(((keypress*)ll->head->ptr)->time);
    now = *((keypress*)ll->head->ptr);
    //printf("%f, %d, %d\n", now.time, now.keycode, now.isUp);
    if(now.keycode < 100 && now.keycode != 28 && now.keycode != 57 && now.keycode != 42){
      array[now.keycode] += 1;
      blah[now.keycode][prev.keycode]++;
      prev = now;
    }
    
    removeFirst(ll);
  }
  int great = 0, gx, gy;
  for(int x = 0; x < 100; x++){
    for(int y = 0; y < 100; y++){
      if(blah[x][y] != 0){
        printf("Keycode %d to %d occured %d times.\n", x, y, blah[x][y]);
        if(great < blah[x][y]){
          great = blah[x][y];
          gx = x;
          gy = y;
        }
      }
    }
  }
  printf("Most common travel is between %d and %d and occured %d times.\n", gy, gx, great);
  
  int first = 0, second = 0, third = 0, fourth = 0, fifth = 0;
  int rfirst, rsecond, rthird = 0, rfourth = 0, rfifth = 0;
  for(int i = 0; i < 100; i++){
    if(array[i] > first){
      fifth = fourth;
      rfifth = rfourth;
      fourth = third;
      rfourth = rthird;
      third = second;
      rthird = rsecond;
      second = first;
      rsecond = rfirst;
      first = array[i];
      rfirst = i;
    }else if(array[i] > second){
      fifth = fourth;
      rfifth = rfourth;
      fourth = third;
      rfourth = rthird;
      third = second;
      rthird = rsecond;
      second = array[i];
      rsecond = i;
    }else if(array[i] > third){
      fifth = fourth;
      rfifth = rfourth;
      fourth = third;
      rfourth = rthird;
      third = array[i];
      rthird = i;
    }else if(array[i] > fourth){
      fifth = fourth;
      rfifth = rfourth;
      fourth = array[i];
      rfourth = i;
    }else if(array[i] > fifth){
      fifth = array[i];
      rfifth = i;
    }
  }
  printf("First: %d\nSecond: %d\nThird: %d\nFourth: %d\nFifth: %d\n-------------------\n", rfirst,rsecond,rthird,rfourth,rfifth);
  printf("First: %d\nSecond: %d\nThird: %d\nFourth: %d\nFifth: %d\n", first,second,third,fourth,fifth);
  return 0;
}
