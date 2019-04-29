#include "maintenanceModule.h"
#include "../../types/types.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if(argc == 2){
    int arg = atoi(argv[1]);
    init_shm();
    if (arg == 1) {
      init_array();
      Program p;
      p.executionTime = 4;
      add_program(p);
      printf("%d\n",get_size() );
    }
    else if(arg == 2){
      del_shm();
    }
    sshmdt();
  }
  else{
    printf("Un seul et unique argument est accepté !\n");
  }

  return 0;
}
