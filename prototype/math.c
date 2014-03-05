#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct{
  double time;
  int keycode;
  int isUp;
}typedef keypress;

struct{
  int size;
  keypress passList [100];
}typedef password;

struct{
  double* averages;
  double* deviation;
}typedef stats;

stats makeStats(password* list [10]){
  int size = list[0]->size;
  double averages [size-1];
  double deviation [size-1];
  
  //Loop for the size of the password.
  for(int x = 0; x < size-1; x++){
    double diff;
    //Loop for all the inputs I'm given.
    for(int y = 0; y < 10; y++){
      diff += list[y]->passList[x+1].time - list[y]->passList[x].time;
    }
    diff /= 10;
    averages[x] = diff;
  }
  
  for(int x = 0; x < size-1; x++){
    double diff = 0;
    double var = 0;
    //Same as previous loops but calcualtes variance & deviation.
    for(int y = 0; y < 10; y++){
      diff += list[y]->passList[x+1].time - list[y]->passList[x].time;
      var += (diff - averages[x])*(diff - averages[x]);
    }
    var /= 10;
    deviation[x] = sqrt(var);
  }
  stats returnMe;
  returnMe.averages = averages;
  returnMe.deviation = deviation;
  return returnMe;
  
}

int sequence(password* old, password* check){
  if(old->size != check->size)
    return 0;
  for(int i = 0; i < old->size; i++){
    if((old->passList[i].keycode != check->passList[i].keycode)
      && (old->passList[i].isUp != old->passList[i].isUp))
      return 0;
  }
  return 1;
}

int acceptable(stats* data, password* toCheck){
  
  return 0;
}


int main(){
  char *stringEnd;
  char str [] = "12386768623.032328665454";
  double num = strtod(str, &stringEnd);
  printf("Fuck you Dylan, %f\n", num);
  
  return 0;
}
