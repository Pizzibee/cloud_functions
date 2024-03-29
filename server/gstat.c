#include <stdio.h>
#include <stdlib.h>
#include "../modules/sharedMem.h"
#include "../types/types.h"

int main(int argc, char const *argv[]) {
  if(argc != 2){
    printf("Un seul et unique argument est accepté !\n");
  }
  else{
    int arg = atoi(argv[1]);
    initShm();
    getSem();
    if(arg >= getSize()) {
      printf("Cet id n'existe pas \n");
    }
    else{
      Program p = getProgram(arg);
      printf("%d\n", p.id);
      printf("%s\n", p.sourceFile);
      if (!p.compilationError) {
        printf("1\n");
        printf("%d\n",p.executionCounter);
        printf("%ld\n",p.executionTime);
      }
      else{
        printf("0\n");
        printf("0\n");
        printf("0\n");
      }
    }
    sshmdt();
  }
  return 0;
}
