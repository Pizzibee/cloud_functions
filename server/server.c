#include <stdio.h>
#include <stdlib.h>
#include "../types/types.h"
#include "../utils/sharedMem.h"

int main(int argc, char const *argv[]) {
  printf("Server hello\n");
  initShm();
  printf("%d\n",getSize() );
  sshmdt();
  return 0;
}
