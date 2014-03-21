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
 *  Requests password entry and trains a distrubution.  
 */

#include <stdint.h>

#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

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

/*
* Name: 
*   makeStats
* Function:
*   Create usefull statistics from a passed in array of password data.
* Preconditions: 
*   Pointer to where the stats should be stored and an array of
*   passwords to create stats from.
* Postconditions:
*   store variable should be updated with stasitical data - 
*   nothing else will change
*/
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

/*
* Name: 
*   acceptable
* Function:
*   Compares a password against a stats structure to detminine if it's
*   within the allowed deviation.  
* Preconditions: 
*   Pointers to both the stats record and the password to check.  
* Postconditions:
*   A 1 is returned if the password is within the acceptable limit
*   a 0 is returned otherwise. 
*/
int acceptable(stats* record, password* check){
  int count = 0;
  FILE* lulz;
  lulz = fopen("testing_data.txt", "w");
  fprintf(lulz, "1");
  for(int i = 0; i < check->size-1; i++){
    double devRoof = record->averages[i]+record->deviation[i];
    double devFloor = record->averages[i]-record->deviation[i];
    double diff = check->passList[i+1].time - check->passList[i].time;
    fprintf(lulz, " %d:%f", i+1, diff);
    if( (devRoof > diff) && ( diff > devFloor) ){
      count++;
    }
  }
  fprintf(lulz, "\n");
  return count;
}

/*
* Name: 
*   sequence
* Function:
*   Compares the sequence of keypresses against the known sequences.
* Preconditions: 
*   The old password to check against and the new password.
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

//Some origional definitoins
#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

int main (int argc, char **argv){
  keypress* keyStruct = malloc(sizeof(keypress));
  password* mainStruct = malloc(sizeof(password));
	password* tempStruct = malloc(sizeof(password));
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
	
  //Make sure there are enough arguments.
	if (argc < 2) {
		printf("Usage: evtest /dev/input/eventX\n");
		printf("Where X = input device number\n");
		return 1;
	}
	//Magic.
	if ((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
		perror("evtest");
		return 1;
	}

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
      } else if (ev[i].type == EV_MSC && (ev[i].code == MSC_RAW 
        || ev[i].code == MSC_SCAN)) {		//do nothing
      } else {
		   //save time as double, keycode as int and state as int
        char timeStr [20];
        sprintf(timeStr, "%ld.%06ld", ev[i].time.tv_sec, ev[i].time.tv_usec);
        char *ptr = NULL;
        double timeDouble = strtod(timeStr, &ptr);
	  		int keycode = ev[i].code;
		    int state = ev[i].value;

		    //printf("Double value, keycode and state = %lf %d %d \n", 
		    //timeDouble, keycode, state);
			
		    keyStruct->time = timeDouble;
        keyStruct->keycode = keycode;
		    keyStruct->isUp = state;
			
		    if(keycode == 28 && state == 1 && count > 9) {
			    //pass in here(new password inside 'keyStruct')
			    tracker = 1;
        }else if(keycode == 28 && state == 1) {
			    k = 0;
			    passArray[count] = *tempStruct;
			    count++;
		    	j = 0;
        }
			  if(keycode !=28 && count > 9) {
          mainStruct->passList[k] = *keyStruct;
          mainStruct->size = k+1;
          k++;
  			}else if(keycode !=28) {
			    tempStruct->passList[j] = *keyStruct;
			    tempStruct->size = j+1;
			    j++;
        }
        if(keycode == 28 && state == 0 && count == 10) {
       	  continue;
		    }
		}
  } //end of while
  //Make stats out of the stored values.
  stats* tmp = malloc(sizeof(stats));
  FILE* fp;
  fp = fopen("training_file.txt", "w");
  for(int dd = 0; dd < 10; dd++){
    fprintf(fp, "1");
    for(int d = 0; d < passArray[dd].size-1; d++){
      fprintf(fp, " %d:%f", d+1, passArray[dd].passList[d+1].time - passArray[dd].passList[d].time);
    }
    fprintf(fp, "\n");
  }
  makeStats(passArray, tmp);
  getNegatives(tmp, fp);
  //---------Stats Created---------	
  printf("--------Averages---------\n");
  for(int d = 0; d < tmp->size; d++){
    printf("%f\n", tmp->averages[d]);
  }
  printf("--------Deviation---------\n");
  for(int d = 0; d < tmp->size; d++){
    printf("%f\n", tmp->deviation[d]);
  }
  printf("--------Upperbound---------\n");
  for(int d = 0; d < tmp->size; d++){
    printf("%f\n", tmp->upperbound[d]);
  }
  printf("--------Lowerbound---------\n");
  for(int d = 0; d < tmp->size; d++){
    printf("%f\n", tmp->lowerbound[d]);
  }
  printf("--------Final Pass---------\n");
  for(int d = 0; d < mainStruct->size; d++){
    printf("%f, %d\n", tempStruct->passList[d].time, tempStruct->passList[d].keycode);
  }
  acceptable(tmp, mainStruct);
  return 0;
}
