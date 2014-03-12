//getPassword function

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
   //1 for yes, 0 for no
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
      printf("j = %d\n", j);
      j++;
    }
  }
  return check;
}

//temp: main() passes its parameters to getPassword to open event path
password* getPassword(int argc, char** argv) {
  keypress* keyStruct = malloc(sizeof(keypress));
  password* passStruct = malloc(sizeof(password));
  password* tempStruct = malloc(sizeof(password));
  //temp: other variable declarations here
  struct input_event eventCap[64];
  int fileDesc, readEvent;
  int addVal = 0;
  int breakVal = 0;
  //temp: get this from readFile method(?)
  int realKeys [] = {35, 18, 38, 38, 24};

  if(argc < 2) {
    printf("Incorrect path used\n");
    return 0;
  }
  if ((fileDesc = open(*argv, O_RDONLY)) < 0) {
		perror("Could not open file");
		return 0;
  }
  printf("\e[1;1H\e[2J");
  printf("Please enter your password: ");

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
        printf("\nState of values = %f %d %d", timeDouble, keycode, state);

        keyStruct->time = timeDouble;
        keyStruct->keycode = keycode;
        keyStruct->isUp = state;
        //ignores the first release of the 'enter' key
        if(keycode == 28 && state == 0) {
          continue;
        }
        else if(keycode == 28 && state == 1) {
          //check. if okay, add to main struct and break. if not, start again
          //temp: check here
          //temp: if not okay..
          int check = checkPassword(tempStruct, realKeys);
          if(check == 0) {
            printf("\nWrong password\n");
          }
          else {
            printf("\nCorrect password");
          }

          //addVal = 0;
          // continue;
          //temp: else, if okay..
          passStruct = tempStruct;
          breakVal = 1;
          break;
        }
        else {
          printf("\naddVal value %d", addVal);
          tempStruct->passList[addVal] = *keyStruct;
          tempStruct->size = addVal+1;
          addVal++;
        }
      }
    }
  }
  close(fileDesc);
  return passStruct;
}

//test: for compilation
int main(int argc, char **argv) {
  password* mainStruct = malloc(sizeof(password));
  mainStruct = getPassword(argc, &argv[1]);
  return 0;
}





    


  
