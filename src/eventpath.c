#include <string.h>
#include <stdio.h>

#define PATH_MAX 104

int main() {
  FILE * filePipe;
  char path[PATH_MAX];
  filePipe = popen("ls -l /dev/input/by-path/platform-i8042-serio-0-event-kbd", "r");  
  fgets(path, PATH_MAX, filePipe);
  printf("This is the path string %s\n", path);
  int eventVal = sizeof(path)-2;
  printf("String size is %d\n", eventVal);
  printf("Last character is %c", path[eventVal]);

  char realPath[] = "/dev/input/event";
  strcat(realPath, &path[eventVal]);
  printf("\nFULL PATH IS %s\n", realPath);
}
