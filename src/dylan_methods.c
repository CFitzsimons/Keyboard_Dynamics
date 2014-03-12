//getPassword and checkPassword functions

#include <stdint.h>
#include <linux/input.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct{
   double time;
   int keycode;
   //1 for down, 0 for up
   int isUp;
}typedef keypress;

struct{
  int size;
  keypress passList [100];
}typedef password;

#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)
#ifndef EV_SYN
#define EV_SYN 0
#endif
    
/*
* Name: 
*   sequence
* Function:
*   Compares the sequence of keypresses against the known sequences.
* Preconditions: 
*   The old password to 
 against and the new password.
* Postconditions:
*   No data will change internally.  The return will be a 1 or a 0
*   indicating whether or not the sequence matches.  
*/
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

/*
* Name: 
*   getPassword
* Function:
*   Acquires the keypresses and associated data from the user's password
* Preconditions: 
*   The path to the event used to gather keypresses
* Postconditions:
*   A password stucture will be filled with a sequence of keypresses
*   representing a password and this will be returned
*/
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
        }
        //if hit enter, store password
        else if(keycode == 28 && state == 1) {
          breakVal = 1;
          break;
        }
        //store each key
        else {
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

void setUp(password passArray[10]) {
  printf("Please enter the password you wish to use: \n");
  password* retStruct = malloc(sizeof(password));
  password allStructs;
  
  char* eventPath = "/dev/input/event2";
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

int main() {
  password* mainStruct = malloc(sizeof(password));
  printf("\e[1;1H\e[2J");
  printf("Please enter your password: \n");
  //password** passArray = malloc(sizeof(password)*10);
  password passArray [10];
  setUp(passArray);

  
  /*
  mainStruct = getPassword(argc, &argv[1]);
  //temp: test phrase is 'hello' with checkPassword
  int realKeys [] = {35, 18, 38, 38, 24};

  int check = checkPassword(mainStruct, realKeys);
  while(check == 0) {
    printf("\e[1;1H\e[2J");
    printf("Wrong password, please try again..\n");
    mainStruct = getPassword(argc, &argv[1]);
    check = checkPassword(mainStruct, realKeys);
  }
  printf("\n\e[1;1H\e[2J");
  printf("Correct password\n");
  //temp: continue onwards(makestats etc)
  */
  return 0;
}

