#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "utils.h"
#include "types.h"

#define SHM_KEY 248
#define SEM_KEY 369
#define PERM 0666

//******************************************************************************
//GLOBAL VARIABLES
//******************************************************************************
int shm_id;
int sem_id;
Program* z;

//******************************************************************************
//SHARED MEMORY
//******************************************************************************
void init_shm() {
  shm_id = shmget(SHM_KEY, sizeof(int), IPC_CREAT | PERM);
  checkNeg(shm_id, "Error shmget");

  z = shmat(shm_id, NULL, 0);
  checkCond(z == (void*) -1, "Error shmat");
}

void sshmdt() {
  int r = shmdt(z);
  checkNeg(r, "Error shmdt");
}

void del_shm() {
  int r = shmctl(shm_id, IPC_RMID, NULL);
  checkNeg(r, "Error shmctl");
}
