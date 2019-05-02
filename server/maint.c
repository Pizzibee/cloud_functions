#include <stdio.h>
#include <stdlib.h>
#include "../utils/sharedMem.h"
#include "../types/types.h"


int main(int argc, char *argv[]) {
  if(argc == 2){
    int arg = atoi(argv[1]);
    initShm();
    if (arg == 1) {
      initArray();
      Program p;
      p.executionTime = 4;
      printf("%d\n", addProgram(p));
      printf("%d\n",getSize() );
    }
    else if(arg == 2){
      delShm();
    }
    sshmdt();
  }
  else{
    printf("Un seul et unique argument est accepté !\n");
  }

  return 0;
}
