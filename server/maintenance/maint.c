#include "maintenanceModule.h"
#include "../../types/types.h"
#include <stdio.h>

int main(int argc, char const *argv[]) {
  init_shm();
  set_id(2);
  sshmdt();
  return 0;
}
