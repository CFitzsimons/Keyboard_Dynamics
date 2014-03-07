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

  if(argc < 2) {
    printf("Incorrect path used\n");
    return 0;
  }
  if ((fileDesc = open(*argv, O_RDONLY)) < 0) {
		perror("Could not open file");
		return 0;
  }

  printf("\n Please enter your password");

  while(breakVal != 1) {
    printf("\nIn while");
    readEvent = read(fileDesc, eventCap, sizeof(struct input_event) * 64);
    
    for(int i = 0; i < readEvent/sizeof(struct input_event); i++) {
      printf("\nIn for");
      if(eventCap[i].type == EV_SYN) {
        printf("\nIn SYN if");
        //do nothing
      } else if(eventCap[i].type == EV_MSC && (eventCap[i].code == MSC_RAW 
               || eventCap[i].code == MSC_SCAN)) {
        printf("\nIn MSC if");
        //do nothing
      }
      else {
        printf("\nIn RIGHT if");
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
  printf("in main");
  printf("\nargc and argv = %d %s", argc, argv[1]);
  password* mainStruct = malloc(sizeof(password));
  mainStruct = getPassword(argc, &argv[1]);
  return 0;
}





    


  
