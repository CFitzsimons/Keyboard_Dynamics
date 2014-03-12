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

//temp: take in password struct and real pw
//..check the down keypress sequence against given password
//..return t or f
int checkPassword(password* checkStruct, int realPass []) {
  int check = 0;
  int i = 0;
  int j = 0;
  while(i <= sizeof(realPass)) {
    //temp: eg hello = 4 size +1 * 2 = 10 = hheelllloo
    if(checkStruct->size != (sizeof(realPass)+1)*2) {
      return 0;
    } 
    if(checkStruct->passList[j].isUp == 1) {
      if(checkStruct->passList[j].keycode == realPass[i]) {
        check = 1;
        i++;
        j++;
      }
      else {
        return 0;
      }
    }
    else {
      j++;
    }
  }
  return check;
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
password* getPassword(int argc, char** argv) {
  keypress* keyStruct = malloc(sizeof(keypress));
  password* passStruct = malloc(sizeof(password));
  struct input_event eventCap[64];
  int fileDesc, readEvent;
  int addVal = 0;
  int breakVal = 0;

  //temp: test this
  if ((fileDesc = open(*argv, O_RDONLY)) < 0) {
		perror("Could not open file");
		return 0;
  }
  //printf("\e[1;1H\e[2J");
  //printf("Please enter your password: ");

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

int main(int argc, char **argv) {
  password* mainStruct = malloc(sizeof(password));
  
  mainStruct = getPassword(argc, &argv[1]);
  //temp: test phrase is 'hello' with checkPassword
  int realKeys [] = {35, 18, 38, 38, 24};

  int check = checkPassword(mainStruct, realKeys);
  while(check == 0) {
    printf("\nWrong password, please try again..\n");
    mainStruct = getPassword(argc, &argv[1]);
    check = checkPassword(mainStruct, realKeys);
  }
  printf("\nCorrect password\n");
  //temp: continue onwards(makestats etc)
  return 0;
}

