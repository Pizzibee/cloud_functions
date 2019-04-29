#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "../../utils/check.h"
#include "../../types/types.h"
#include "maintenanceModule.h"

#define SHM_KEY 240
#define SEM_KEY 369
#define PERM 0666

//******************************************************************************
//GLOBAL VARIABLES
//******************************************************************************
int shm_id;
ProgramArray* tab;

//******************************************************************************
//SHARED MEMORY
//******************************************************************************
void init_shm() {
  shm_id = shmget(SHM_KEY, sizeof(int), IPC_CREAT | PERM);
  checkNeg(shm_id, "Error shmget");

  tab = shmat(shm_id, NULL, 0);
  checkCond(tab == (void*) -1, "Error shmat");
}

void sshmdt() {
  int r = shmdt(tab);
  checkNeg(r, "Error shmdt");
}

void del_shm() {
  int r = shmctl(shm_id, IPC_RMID, NULL);
  checkNeg(r, "Error shmctl");
}

void init_array(){
  (tab->size) = 0;
}

int get_size(){
  return tab->size;
}

void add_program(Program p) {
  int s = tab->size;
  p.id = s;
  (tab->tabProg)[s] = p;
  tab->size = s+1;
}

Program* get_array(){
  return tab->tabProg;
}
