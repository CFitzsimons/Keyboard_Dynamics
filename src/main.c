#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)
#ifndef EV_SYN
#define EV_SYN 0
#endif

//Structure to hold data related to a single key event
struct{
   double time;
   int keycode;
   int isUp;
}typedef keypress;

//Structure to hold a password using an unfilled array.
struct{
  int size;
  keypress passList [100];
}typedef password;

//Structure to hold statistics related to a given array.  
struct{
  int size;
  double averages [100];
  double deviation [100];
  double upperbound[100];
  double lowerbound[100];
  double firstquart[100];
  double thirdquart[100];
}typedef stats;

password* getPassword(char* eventPath) {
  keypress* keyStruct = malloc(sizeof(keypress));
  password* passStruct = malloc(sizeof(password));
  struct input_event eventCap[64];
  int fileDesc, readEvent;
  int addVal = 0;
  int breakVal = 0;

  //temp: test this
  if ((fileDesc = open(eventPath, O_RDONLY)) < 0) {
		perror("Could not open file");
		return 0;
  }

  while(breakVal != 1) {
    readEvent = read(fileDesc, eventCap, sizeof(struct input_event) * 64);
    
    for(int i = 0; i < readEvent/sizeof(struct input_event); i++) {
      if(eventCap[i].type == EV_SYN) {
        //do nothing
      } else if(eventCap[i].type == EV_MSC && (eventCap[i].code == MSC_RAW 
               || eventCap[i].code == MSC_SCAN)) {
        //do nothing
      }
      else {
        //temp: change size to fit better
        char timeString [20];
        sprintf(timeString, "%ld.%06ld", eventCap[i].time.tv_sec, eventCap[i].time.tv_usec);
        char *ptr = NULL;
        double timeDouble = strtod(timeString, &ptr);
        int keycode = eventCap[i].code;
        int state = eventCap[i].value;

        keyStruct->time = timeDouble;
        keyStruct->keycode = keycode;
        keyStruct->isUp = state;
        //ignores the first release of the 'enter' key
        if(keycode == 28 && state == 0) {
          continue;
        }else if(keycode == 28 && state == 1) {
          breakVal = 1;
          break;
        }else {
          passStruct->passList[addVal] = *keyStruct;
          passStruct->size = addVal+1;
          addVal++;
        }
      }
    }
  }
  close(fileDesc);
  return passStruct;
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

void readStats(stats* read){
  FILE* fp;
  fp = fopen("data.bin", "rb");
  fflush(fp);
  fseek(fp, sizeof(password), SEEK_CUR);
  fread(read, sizeof(stats), 1, fp);
  fclose(fp);
}

void readPass(password* read){
  FILE* fp;
  fp = fopen("data.bin", "rb");
  fflush(fp);
  fread(read, sizeof(password), 1, fp);
  fclose(fp);
}

int writeData(password* writePass, stats* writeStats){
  FILE* fp;
  fp = fopen("data.bin", "wb");
  fflush(fp);
  fwrite(writePass, sizeof(password), 1, fp);
  fwrite(writeStats, sizeof(stats), 1, fp);
  fclose(fp);
  return 1;
}

int svmCall(){
  //execl("./easy.py", "training.txt", "data.txt", NULL);
}

stats* makeStats(password list [10], stats* store){
  store->size = 0;
  int size = list[0].size;
  double averages [size-1];
  double deviation [size-1];
  //loop to calcuate the averages
  for(int x = 0; x < size-1; x++){
    double diff = 0;
    //Loop for all the inputs I'm given.
    //fprintf(fp, "1");
    double tmp = 0;
    for(int y = 0; y < 10; y++){
      tmp = list[y].passList[x+1].time - list[y].passList[x].time;
      if(y == 0){
        store->upperbound[x] = tmp;
        store->lowerbound[x] = tmp;
      }else if(store->upperbound[x] < tmp){
        store->upperbound[x] = tmp;
      }else if(store->lowerbound[x] > tmp){
        store->lowerbound[x] = tmp;
      }
      diff += tmp;
    }
    printf("Difference %f -- -- ", diff);
    diff /= 10;
    printf("Difference %f\n", diff);
    store->averages[x] = diff;
    store->averages[x] = diff;
    store->thirdquart[x] = store->upperbound[x] - store->averages[x];
    store->firstquart[x] = store->lowerbound[x] + store->averages[x];
  }
  //fclose(fp);
  
  store->size = size-1;
  
  for(int x = 0; x < size-1; x++){
    double diff = 0;
    double var = 0;
    //Same as previous loops but calcualtes variance & deviation.
    for(int y = 0; y < 10; y++){
      diff = list[y].passList[x+1].time - list[y].passList[x].time;
      var += (diff - averages[x])*(diff - averages[x]);
    }
    var /= 10;
    store->deviation[x] = sqrt(var);
  }
  return store;
}

int getPositives(password list [10], FILE* fp){
  for(int i = 0; i < 10; i++){
    fprintf(fp, "1");
    for(int j = 0; j < list[i].size-1; j++){
      double diff = list[i].passList[j+1].time - list[i].passList[j].time;
      fprintf(fp, " %d:%f", j+1, diff);
    }
    fprintf(fp, "\n");
  }
  return 1;
}

double randfrom(double min, double max) {
  double range = (max - min); 
  double div = RAND_MAX / range;
  return min + (rand() / div);
}

int getNegatives(stats* ptr, FILE* fp){
  for(int x = 0; x < 12; x++){
    double move = 0;
    if(x == 0 || x % 4 == 0) //Stop divide by zero
      move = randfrom(ptr->thirdquart[0],ptr->upperbound[0]);
    else if(x % 4 == 1)
      move = randfrom(ptr->lowerbound[0], ptr->firstquart[0]);
    else if(x % 4 == 2)
      move = ptr->upperbound[0] - ptr->lowerbound[0];
    else if(x % 4 == 3)
      move = ptr->lowerbound[0] - ptr->upperbound[0];
      
    fprintf(fp, "-1");
    for(int i = 0; i < ptr->size; i++){
      double skew = randfrom(ptr->firstquart[i], ptr->thirdquart[i]);
      
      fprintf(fp, " %d:%f", i+1, skew + move);
    }
    fprintf(fp, "\n");
  }
}

int getDataFile(password* check){
  FILE* fp;
  fp = fopen("data.txt", "w");
  fprintf(fp, "1");
  for(int i = 0; i < check->size-1; i++){
    double diff = check->passList[i+1].time - check->passList[i].time;
    fprintf(fp, " %d:%f", i+1, diff);
  }
  fprintf(fp, "\n");
  return 1;
}


void setUp(password passArray[10]) {
  printf("Please enter the password you wish to use: \n");
  password* retStruct = malloc(sizeof(password));
  password allStructs;
  
  char* eventPath = "/dev/input/event4";
  int check = 1;
  int i = 0;
  retStruct = getPassword(eventPath);
  printf("\nPlease enter the password 10 times\n");
  while(i < 10) {
    allStructs = *getPassword(eventPath);
    check = sequence(retStruct, &allStructs);
    if(check == 1) {
      passArray[i] = allStructs;
      i++;
    }else {
      printf("\nWrong password, please try again\n");
    }
  }
}

int main(){
  password* pass = malloc(sizeof(password));
  stats* info = malloc(sizeof(stats));
  password passList [10];
  if(access("data.bin", F_OK ) != -1 ){
    readPass(pass);
    readStats(info);
  }else{
    setUp(passList);
    *pass = passList[0];
    makeStats(passList, info);
  }
  FILE* fp;
  fp = fopen("training.txt", "w");
  getPositives(passList, fp);
  getNegatives(info, fp);
  fclose(fp);
  password* user = malloc(sizeof(password));
  while(1){
    user = getPassword("/dev/input/event4");
    if(sequence(user, pass) == 1){
      printf("Correct password\n");
      getDataFile(user);
      svmCall(); 
      //writeData(pass, info);
      exit(1);
    }
  }
}

