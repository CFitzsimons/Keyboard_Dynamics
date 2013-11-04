/*
 * $Id: evtest.c,v 1.23 2005/02/06 13:51:42 vojtech Exp $
 *
 *  Origional work by:
 *  Copyright (c) 1999-2000 Vojtech Pavlik
 *
 *  Modified: Event device test program
 *
 *  Modified by Colin Fitzsimons and Dylan Lee
 *  
 *  Requests password entry and trains a graph.  
 */

#include <stdint.h>

#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef EV_SYN
#define EV_SYN 0
#endif
 


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

struct{
  double* averages;
  double* deviation;
}typedef stats;

stats* makeStats(password list [10], stats* store){
  store = malloc(sizeof(stats));  
  int size = list[0].size;
  double averages [size-1];
  double deviation [size-1];
  
  //Loop for the size of the password.
  for(int x = 0; x < size-1; x++){
    double diff = 0;
    //Loop for all the inputs I'm given.
    for(int y = 0; y < 10; y++){
      diff += list[y].passList[x+1].time - list[y].passList[x].time;
    }
    printf("Difference %f -- -- ", diff);
    diff /= 10;
    printf("Difference %f\n", diff);
    averages[x] = diff;
  }
  
  for(int x = 0; x < size-1; x++){
    double diff = 0;
    double var = 0;
    //Same as previous loops but calcualtes variance & deviation.
    for(int y = 0; y < 10; y++){
      diff += list[y].passList[x+1].time - list[y].passList[x].time;
      var += (diff - averages[x])*(diff - averages[x]);
    }
    var /= 10;
    deviation[x] = sqrt(var);
  }
  store->averages = averages;
  store->deviation = deviation;
  for(int i = 0; i < sizeof(store->averages); i++){
    printf("%f\n", store->averages[i]);
  }
  return store;
}

int acceptable(stats* record, password* check){
  for(int i = 0; i < check->size-1; i++){
    if( (record->averages[i]+record->deviation[i] > check->passList[i+1].time-check->passList[i].time) 
      || (record->averages[i]-record->deviation[i] < check->passList[i+1].time-check->passList[i].time) )
      return 0;
  }
  return 1;
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
#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

int main (int argc, char **argv){
  keypress* keyStruct = malloc(sizeof(keypress));
  password* mainStruct = malloc(sizeof(keypress));
	password* tempStruct = malloc(sizeof(keypress));
	int fd, rd, i;
	int j = 0;
	int k = 0;
	int count = 0;
	int tracker = 0;
	password passArray [10];
	struct input_event ev[64];
	int version;
	unsigned short id[4];
	unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
	char name[256] = "Unknown";
	int abs[5];

	if (argc < 2) {
		printf("Usage: evtest /dev/input/eventX\n");
		printf("Where X = input device number\n");
		return 1;
	}

	if ((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
		perror("evtest");
		return 1;
	}

	if (ioctl(fd, EVIOCGVERSION, &version)) {
		perror("evtest: can't get version");
		return 1;
	}

	/*printf("Input driver version is %d.%d.%d\n",
		version >> 16, (version >> 8) & 0xff, version & 0xff);
*/

	ioctl(fd, EVIOCGID, id);
	/*printf("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
		id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]);
*/
	ioctl(fd, EVIOCGNAME(sizeof(name)), name);
	//printf("Input device name: \"%s\"\n", name);

	memset(bit, 0, sizeof(bit));
	ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]);
	//printf("Supported events:\n");

	printf("Please enter your password\n");

	//read in data
        
    while (count < 10 || tracker < 1) {
		rd = read(fd, ev, sizeof(struct input_event) * 64);

		if (rd < (int) sizeof(struct input_event)) {
			printf("yyy\n");
			perror("\nevtest: error reading");
			return 1;
		}


      for (i = 0; i < rd / sizeof(struct input_event); i++)
		    if(ev[i].type == EV_SYN) {
			//do nothing
		    } else if (ev[i].type == EV_MSC && (ev[i].code == MSC_RAW || ev[i].code == MSC_SCAN)) {		//do nothing
			  } else {
			    //save time as double, keycode as int and state as int
			    char timeStr [20];
 			    sprintf(timeStr, "%ld.%06ld", ev[i].time.tv_sec, ev[i].time.tv_usec);
			    char *ptr = NULL;
			    double timeDouble = strtod(timeStr, &ptr);
			    int keycode = ev[i].code;
			    int state = ev[i].value;

			    printf("Double value, keycode and state = %lf %d %d \n", timeDouble, keycode, state);
			
			    keyStruct->time = timeDouble;
			    keyStruct->keycode = keycode;
			    keyStruct->isUp = state;
			
			    if(keycode == 28 && state == 1 && count > 9) {
			    //pass in here(new password inside 'keyStruct')
			    tracker = 1;
			    }
			    else if(keycode == 28 && state == 1) {
			    int k;
			    for(k = 0; k < j; k++) {
			      printf("\ntempStruct time, type, keycode vals = %f %d %d " , (tempStruct->passList[k]).time , (tempStruct->passList[k]).isUp , (tempStruct->passList[k]).keycode);
          }
			    passArray[count] = *tempStruct;
			    count++;
		    	    j = 0;
      }
			if(keycode !=28 && count > 9) {
			  mainStruct->passList[k] = *keyStruct;
			  mainStruct->size = k+1;
			  k++;
			}
			//save inside temp struct
			else if(keycode !=28) {
			  tempStruct->passList[j] = *keyStruct;
			  tempStruct->size = j+1;
			  j++;
			}
			if(keycode == 28 && state == 0 && count == 10) {
			  continue;
			}
		}
			
  } //end of while
  //All data is stored at this point.  
  //---------Create Stats----------
  stats* tmp = malloc(sizeof(stats));
  tmp = makeStats(passArray, tmp);
  //---------Stats Created---------	
  
  return 0;
}




  




