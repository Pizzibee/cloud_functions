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
      delShm();
      delSem();
      initSem(1);
      initArray();
      printf("La mémoire partagée a été créée\n");
    }
    else if(arg == 2){
      delShm();
      delSem();
      printf("La mémoire partagée a été détruite \n");
    }
    sshmdt();
  }
  else{
    if (argc == 3) {
      int arg1 = atoi(argv[1]);
      int arg2 = atoi(argv[2]);
      initShm();
      getSem();
      if(arg1 == 3){
        printf("La mémoire partagée est réservée pendant %d secondes\n", arg2);
        semBlock(arg2);
        printf("DONE.\n");
      }
      sshmdt();
    }
  }

  return 0;
}
