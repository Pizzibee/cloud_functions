#include <stdio.h>
#include <stdlib.h>
#include "../types/types.h"
#include "../server/maintenance/maintenanceModule.h"

int main(int argc, char const *argv[]) {
  printf("Server hello\n");
  init_shm();
  printf("");
  sshmdt();
  return 0;
}
