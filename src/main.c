#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <stdint.h>
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>

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
  printf("Sizes %d, %d: \n", old->size, check->size);
  if(old->size != check->size)
    return 0;
  for(int i = 0; i < old->size; i++){
    if((old->passList[i].keycode != check->passList[i].keycode)
      && (old->passList[i].isUp != old->passList[i].isUp))
      return 0;
  }
  return 1;
}

void readPass(password read [20]){
  FILE* fp;
  fp = fopen("data.bin", "rb");
  fflush(fp);
  fread(read, sizeof(password), 20, fp);
  fclose(fp);
}

int writeData(password writePass [20]){
  FILE* fp;
  fp = fopen("data.bin", "wb");
  fflush(fp);
  fwrite(writePass, sizeof(password), 20, fp);
  fclose(fp);
  return 1;
}

stats* makeStats(password list [20], stats* store){
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
    for(int y = 0; y < 20; y++){
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
    diff /= 20;
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
    for(int y = 0; y < 20; y++){
      diff = list[y].passList[x+1].time - list[y].passList[x].time;
      var += (diff - averages[x])*(diff - averages[x]);
    }
    var /= 20;
    store->deviation[x] = sqrt(var);
  }
  return store;
}

int getPositives(password list [20], FILE* fp){
  for(int i = 0; i < 20; i++){
    fprintf(fp, "1");
    for(int j = 0; j < list[i].size-1; j++){
      double diff = list[i].passList[j+1].time - list[i].passList[j].time;
      fprintf(fp, " %d:%f", j+1, diff);
    }
    fprintf(fp, "\n");
  }
  return 1;
}

int getDataFile(password* check){
  FILE* fp;
  fp = fopen("data.txt", "a");
  fprintf(fp, "1");
  for(int i = 0; i < check->size-1; i++){
    double diff = check->passList[i+1].time - check->passList[i].time;
    fprintf(fp, " %d:%f", i+1, diff);
  }
  fprintf(fp, "\n");
  fclose(fp);
  return 1;
}


void setUp(password passArray[20]) {
  printf("Please enter the password you wish to use: \n");
  password* retStruct = malloc(sizeof(password));
  password allStructs;
  
  char* eventPath = "/dev/input/event4";
  int check = 1;
  int i = 0;
  retStruct = getPassword(eventPath);
  printf("\nPlease enter the password 20 times\n");
  while(i < 20) {
    if(i == 0){
      system("clear");
      printf("Please enter your password 10 times\n");
    }else if(i == 10){
      printf("Data collected...\n");
      sleep(2);
      system("clear");
      printf("Please enter your password 5 times, as fast as you can\n");
    }else if(i == 15){
      printf("Data collected...\n");
      sleep(2);
      system("clear");
      printf("Please enter your password 5 times, paced out\n");
    }
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
  sigset_t mainmask, waitingmask;
  sigemptyset(&mainmask);
  sigemptyset(&waitingmask);
  sigaddset(&mainmask, SIGINT);
  //block signal being sent to kill threads
  pthread_sigmask(SIG_BLOCK, &mainmask, 0);
  
  password* pass = malloc(sizeof(password));
  stats* info = malloc(sizeof(stats));
  FILE* plot;
  //plot = fopen("plot.csv", "w");
  password passList [20];
  if(access("data.bin", F_OK ) != -1 ){
    readPass(passList);
  }else{
    setUp(passList);
  }
  *pass = passList[0];
  makeStats(passList, info);
  FILE* fp;
  fp = fopen("training.txt", "w");
  getPositives(passList, fp);
  //getNegatives(info, fp);
  fclose(fp);
  password user;
  int check = 0;
  while(!sigismember(&waitingmask, SIGINT)){
    user = *getPassword("/dev/input/event4");
    sigpending(&waitingmask);
    if(sequence(pass, &user) == 1){
      plot = fopen("plot.csv", "w");
      printf("\nCorrect password\n");
      getDataFile(&user);
      writeData(passList);
      if(check == 0){
        system("./svm-train -s 2 training.txt model.txt");
        check = 1;
      }
      system("./svm-predict data.txt model.txt answers.txt");
      for(int i = 0; i < info->size;i++){
        if(i != info->size){
          fprintf(plot, "%f,", info->averages[i]);
          fprintf(plot, "%f\n", user.passList[i+1].time-user.passList[i].time);
        }else{
          fprintf(plot, "%f\n", info->averages[i]);
          fprintf(plot, "%f\n", user.passList[i+1].time-user.passList[i].time);
        }
      }
      fclose(plot);
      system("R CMD BATCH sample.r");
    }else{
      printf("\nWrong password, please try again\n");
      continue;
    }
  }
  printf("Exiting gracefully...\n");
}

