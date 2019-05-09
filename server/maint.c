#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../modules/sharedMem.h"
#include "../types/types.h"


int main(int argc, char *argv[]) {
  if(argc == 2){
    int arg = atoi(argv[1]);
    initShm();
    getSem();
    if (arg == 1) {
      initSem(1);
      initArray();
      Program p1;
      strcpy(p1.sourceFile,"test1.c");
      p1.compilationError = false;
      p1.executionCounter = 4;
      p1.executionTime = 20;
      Program p2;
      strcpy(p2.sourceFile,"test2.c");
      p2.compilationError = true;
      p2.executionCounter = 4;
      p2.executionTime = 20;
      printf("%d\n", addProgram(p1));
      printf("%d\n", addProgram(p2));
    }
    else if(arg == 2){
      delShm();
      delSem();
    }
    sshmdt();
  }
  else{
    printf("Un seul et unique argument est accepté !\n");
  }

  return 0;
}
