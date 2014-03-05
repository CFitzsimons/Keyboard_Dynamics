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

void makeStats(password* list [10]){
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
}

int power(int i){
  for(int a = 0; a < i; a++){
    
  }
}

long double parseDouble(char str []){
  int i = 0;
  int right = -1;
  long double full = 0;
  for(i = 0; i < sizeof(str); i++){
    if(str[i] == '.'){
      right++;
    }else if(right != -1){
      right++;
      full += (pow(10,sizeof(str)+1-i))*(str[i]-48);
    }else{
      full += (pow(10,sizeof(str)-i))*(str[i]-48);
    }
    printf("\n%f, %c\n", full, str[i]);
  }
  full *= pow(10, right);
  return full;
}

int main(){
  char *stringEnd;
  char str [] = "12386768623.032328665454";
  double num = strtod(str, &stringEnd);
  printf("Fuck you Dylan, %f\n", num);
  
  return 0;
}
