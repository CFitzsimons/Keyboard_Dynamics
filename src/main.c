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

//Constant definitions.
#define PATH_MAX 104 
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
  double deviation[100];
  double upperbound[100];
  double lowerbound[100];
  double firstquart[100];
  double thirdquart[100];
}typedef stats;


/*
* Name:
*   getPassword
* Purpose:
*   Retrieve a password from device passed in though arguments.
* Preconditions:
*   Requires a path to the device to listen on.
* Postconditions:
*   A pointer to a password generated by the user.
*/
password* getPassword(char* realPath) {
  keypress* keyStruct = malloc(sizeof(keypress));
  password* passStruct = malloc(sizeof(password));
  struct input_event eventCap[64];
  int fileDesc, readEvent;
  int addVal = 0;
  int breakVal = 0;

  if ((fileDesc = open(realPath, O_RDONLY)) < 0) {
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
        //converts time string into a double
        char timeString [20];
        sprintf(timeString, "%ld.%06ld", eventCap[i].time.tv_sec, 
          eventCap[i].time.tv_usec);
        char *ptr = NULL;
        double timeDouble = strtod(timeString, &ptr);
        int keycode = eventCap[i].code;
        int state = eventCap[i].value;

        keyStruct->time = timeDouble;
        keyStruct->keycode = keycode;
        keyStruct->isUp = state;
        //ignores the release of the 'enter' key
        if(keycode == 28 && state == 0) {
          continue;
        //break out if user hits the 'enter' key
        }else if(keycode == 28 && state == 1) {
          breakVal = 1;
          break;
        }else {
          //otherwise, store keypress
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
/*
* Name:
*   sequence
* Purpose:
*   Checks the sequence of keypresses in a password.
* Preconditions:
*   The old and new password to check.
* Postconditions:
*   A 1 if the password is alright, a 0 otherwise.  
*/
int sequence(password* old, password* check){
  printf("Sizes %d, %d: \n", old->size, check->size);
  if(old->size != check->size)
    return 0;
  //This section is commented out during testing and presentation but would be
  //fully enabled during use.  
  //for(int i = 0; i < old->size; i++){
  //  if((old->passList[i].keycode != check->passList[i].keycode)
  //    || (old->passList[i].isUp != old->passList[i].isUp))
  //    return 0;
  //}
  return 1;
}

/*
* Name:
*   readPass
* Purpose:
*   Reads an array of n passwords from a file
* Preconditions:
*   The array you wish to store the data into.
* Postconditions:
*   Stores the data into the passed in array.
*/
void readPass(password read [20]){
  FILE* fp;
  fp = fopen("data.bin", "rb");
  fflush(fp);
  fread(read, sizeof(password), 20, fp);
  fclose(fp);
}

/*
* Name:
*   writeData
* Purpose:
*   Write data passed in though args to a file.
* Preconditions:
*   The data to be written
* Postconditions:
*   A status indicating that the write was sucessfull.
*/
int writeData(password writePass [20]){
  FILE* fp;
  fp = fopen("data.bin", "wb");
  fflush(fp);
  fwrite(writePass, sizeof(password), 20, fp);
  fclose(fp);
  return 1;
}

/*
* Name:
*   makeStats
* Purpose:
*   Creates a strucure of usefull statistics based upon a list of passwords
*   passed in though arguments.
* Preconditions:
*   The array of password data and a preconstructed stats structures to hold
*   the data that is read in.
* Postconditions:
*   The stats structure is modifed to contain usefull statistical information.
*/
stats* makeStats(password list [20], stats* store){
  store->size = 0;
  int size = list[0].size;
  double averages [size-1];
  double diff, tmp;
  //loop to calcuate the averages
  for(int x = 0; x < size-1; x++){
    diff = 0;
    tmp = 0;
    //Loop for all the inputs I'm given.
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
    //Store the collected data into the store variable.
    store->averages[x] = diff;
    store->averages[x] = diff;
    store->thirdquart[x] = store->upperbound[x] - store->averages[x];
    store->firstquart[x] = store->lowerbound[x] + store->averages[x];
  }
  store->size = size-1;
  
  for(int x = 0; x < size-1; x++){
    diff = 0;
    tmp = 0;
    //Same as previous loops but calcualtes variance & deviation.
    for(int y = 0; y < 20; y++){
      diff = list[y].passList[x+1].time - list[y].passList[x].time;
      tmp += (diff - averages[x])*(diff - averages[x]);
    }
    tmp /= 20;
    store->deviation[x] = sqrt(tmp);
  }
  return store;
}

/*
* Name:
*   getPositive
* Purpose:
*   Write a sequence of 20 passwords in a format that libSVM can read.
* Preconditions:
*   The list of passwords and a pointer to the file to be written to.
* Postconditions:
*   A status indicating if the write was sucessfull or not.  
*/
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

/*
* Name:
*   putDataFile
* Purpose:
*   Appends the latest password attempt to the data file in a format readable
*   by libSVM.
* Preconditions:
*   A pointer to the password we wish to append.
* Postconditions:
*   A status indicating whether or not the write was sucessfull.  
*/
int putDataFile(password* check){
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

/*
* Name:
*   setUp
* Purpose:
*   Reads 20 passwords from the user, prompting them each time for more data.  
*   This method should only be called if new data is needed or the first time
*   the program is ran.
* Preconditions:
*   A password array to hold the 20 pieces of data and a path to the device to
*   read from.  
* Postconditions:
*   Modifes the passed in array to hold the users data.
*/
void setUp(password passArray[20], char * realPath) {
  printf("Please enter the password you wish to use: \n");
  password* retStruct = malloc(sizeof(password));
  password allStructs;
  int check = 1;
  int i = 0;
  //Call getPassword and give it the path to the input device
  retStruct = getPassword(realPath);
  //Request the password 20 times.  
  printf("\nPlease enter the password 20 times\n");
  while(i < 20) {
    if(i == 0){
      //Request the password 10 times at normal speed.
      system("clear");
      printf("Please enter your password 10 times\n");
    }else if(i == 10){
      //Request the data 5 times at a fast pace.  
      printf("Data collected...\n");
      sleep(2);
      system("clear");
      printf("Please enter your password 5 times, as fast as you can\n");
    }else if(i == 15){
      printf("Data collected...\n");
      //Request the data 5 times at a slower pace.  
      sleep(2);
      system("clear");
      printf("Please enter your password 5 times, paced out\n");
    }
    //Save the new password into a structure and check if the sequence is
    //correct.
    allStructs = *getPassword(realPath);
    check = sequence(retStruct, &allStructs);
    //If the password was entered correctly, add one to i and continue.  
    if(check == 1) {
      passArray[i] = allStructs;
      i++;
    }else {
      printf("\nWrong password, please try again\n");
      
    }
  }
}

int main(){
  srand(time(NULL));
  //Setup for blocking SIGINT.
  sigset_t mainmask, waitingmask;
  sigemptyset(&mainmask);
  sigemptyset(&waitingmask);
  sigaddset(&mainmask, SIGINT);
  pthread_sigmask(SIG_BLOCK, &mainmask, 0);
  //Declaring some variables for use later.
  password* pass = malloc(sizeof(password));
  stats* info = malloc(sizeof(stats));
  password passList [20];
  int check = 0;
  password user;
  FILE* plot;
  FILE* filePipe;
  FILE* fp;
  //Detecting which device to listen on.
  char path[PATH_MAX];
  
  filePipe = popen("ls -l /dev/input/by-path/platform-i8042-serio-0-event-kbd",
     "r");  
  fgets(path, PATH_MAX, filePipe);
  int eventVal = sizeof(path)-2;
  char realPath[] = "/dev/input/event";
  strcat(realPath, &path[eventVal]);
  //Check if the program has been setup already.
  if(access("data.bin", F_OK ) != -1 ){
    readPass(passList);
  }else{
    setUp(passList, realPath);
  }
  *pass = passList[0];
  makeStats(passList, info);
  //Open up training.txt for writing to.  
  fp = fopen("training.txt", "w");
  //Get and store positive exemplars in a format libSVM can read.  
  getPositives(passList, fp);
  fclose(fp);
  //Look and request data from the user until SIGINT is fired.
  printf("Begining analysis, enter in passwords.  ");
  printf("When you are done, send an interrupt sigal using CTRL + C\n");
  while(!sigismember(&waitingmask, SIGINT)){
    user = *getPassword(realPath);
    //Check if a siganl is pending
    sigpending(&waitingmask);
    //If the sequence is correct, ask the user for 
    if(sequence(pass, &user) == 1){
      //Open up the CSV file for writing
      plot = fopen("plot.csv", "w");
      printf("\nCorrect password\n");
      putDataFile(&user);
      writeData(passList);
      //If a model file hasn't been created this run, create one.
      if(check == 0){
        //A call to libSVM to create a one class model.
        system("./svm-train -s 2 training.txt model.txt");
        check = 1;
      }
      //Call libSVM and pass in the created data and model file
      system("./svm-predict data.txt model.txt answers.txt");
      //Write data to the CSV file for use by R
      for(int i = 0; i < info->size;i++){
        fprintf(plot, "%f,", passList[5].passList[i+1].time-passList[5].passList[i].time);
        fprintf(plot, "%f,", passList[13].passList[i+1].time-passList[13].passList[i].time);
        fprintf(plot, "%f,", passList[17].passList[i+1].time-passList[17].passList[i].time);
        fprintf(plot, "%f\n", user.passList[i+1].time-user.passList[i].time);
        
      }
      //Close the write to the CSV file
      fclose(plot);
      //Call R to create two boxplots relative to the current data.  
      system("R CMD BATCH boxplot.r");
    }else{
      printf("\nWrong password, please try again\n");
      continue;
    }
  }
  printf("Exiting gracefully...\n");
}


