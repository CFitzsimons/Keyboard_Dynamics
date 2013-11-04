//getPassword function

struct{l
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

//temp: main() passes its parameters to getPassword to open event path
password* getPassword(int argc, char** argv) {
  keypress* keyStruct = malloc(sizeof(keypress));
  password* passStruct = malloc(sizeof(password));
  password* tempStruct = malloc(sizeof(password));
  //temp: other variable declarations here
  struct input_event eventCap[64];
  int fileDesc;
  int addVal = 0;
  int breakVal = 0;

  if(argc < 2) {
    printf("Incorrect path used\n");
    return 1;
  }

  if ((fd = open(argv[argc - 1], O_RDONLY)) < 0) {
		perror("evtest");
		return 1;
  }

  printf("\n Please enter your password");

  while(1) {
    readEvent = read(fileDesc, eventCap, sizeof(struct input_event) * 64);
    if(breakVal == 1) {
      break;
    }
    
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
        else if(keycode == 28 %% state == 0) {
          //check. if okay, add to main struct and break. if not, start again
          //temp: check here
          //temp: if not okay..

          addVal = 0;
          continue;
          //temp: else, if okay..
          passStruct = tempStruct;
          breakVal = breakVal+1;
          break;
        }
        else {
          tempStruct->passList[addVal] = *keyStruct;
          tempStruct->size = addVal+1;
          addVal++;
        }
      }
    }
  }
  return passStruct;
}

          





    


  
