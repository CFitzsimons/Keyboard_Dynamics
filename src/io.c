#include <stdio.h>

void readStats(stats* read){
  FILE* fp;
  fp = fopen("data.bin", "rb");
  fflush(fp);
  fseek(fp, sizeof(password), SEEK_CUR);
  fread(read, sizeof(stats), 1, fp);
  fclose(fp);
}

void readPass(password* read){
  FILE* fp;
  fp = fopen("data.bin", "rb");
  fflush(fp);
  fread(read, sizeof(password), 1, fp);
  fclose(fp);
}

int writeData(password* writePass, stats* writeStats){
  FILE* fp;
  fp = fopen("data.bin", "wb");
  fflush(fp);
  fwrite(writePass, sizeof(password), 1, fp);
  fwrite(writeStats, sizeof(stats), 1, fp);
  fclose(fp);
  return 1;
}

int main(){


}


