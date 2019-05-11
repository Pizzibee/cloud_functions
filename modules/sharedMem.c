#include "check.h"
#include "../types/types.h"
#include "sharedMem.h"

#define SHM_KEY 241
#define SEM_KEY 368
#define PERM 0666


//******************************************************************************
//GLOBAL VARIABLES
//******************************************************************************
int shm_id;
int sem_id;
ProgramArray* tab;

//******************************************************************************
//SHARED MEMORY
//******************************************************************************
void initShm() {
  shm_id = shmget(SHM_KEY, sizeof(ProgramArray), IPC_CREAT | PERM);
  checkNeg(shm_id, "Error shmget");

  tab = shmat(shm_id, NULL, 0);
  checkCond(tab == (void*) -1, "Error shmat");
}

void sshmdt() {
  int r = shmdt(tab);
  checkNeg(r, "Error shmdt");
}

void delShm() {
  int r = shmctl(shm_id, IPC_RMID, NULL);
  checkNeg(r, "Error shmctl");
}

void initArray(){
  down();
  tab->size = 0;
  up();
}

int getSize(){
  down();
  int res = tab->size;
  up();
  return res;
}

int addProgram(Program p) {
  down();
  p.id = tab->size;
  (tab->tabProg)[tab->size] = p;
  (tab->size) = tab->size + 1;
  up();
  return p.id;
}

Program getProgram(int index){
  down();
  Program p = (tab->tabProg)[index];
  up();
  return p;

}

void setProgram(int id, Program p){
  down();
  (tab->tabProg)[id] = p;
  up();
}


//******************************************************************************
//SEMAPHORE
//******************************************************************************


void initSem(int val) {

  // CREATE A SET OF ONE SEMAPHORE.
  // THE NUMBER ASSOCIATED WITH THIS SEMAPHORE IS 0.
  sem_id = semget(SEM_KEY, 1, IPC_CREAT | PERM);
  checkNeg(sem_id, "Error semget");

  // INIT THE SEMAPHORE VALUE TO val
  union semun arg;
  arg.val = val;

  int rv = semctl(sem_id, 0, SETVAL, arg);
  checkNeg(rv, "Error semctl");
}

void getSem(){
  sem_id = semget(SEM_KEY, 1, IPC_CREAT | PERM);
  checkNeg(sem_id, "Error semget");
}

void addSem(int val) {
  struct sembuf sem;
  sem.sem_num = 0;
  sem.sem_op = val;
  sem.sem_flg = 0;

  int rc = semop(sem_id, &sem, 1);
  checkNeg(rc, "Error semop");
}

void down() {
  addSem(-1);
}

void up() {
  addSem(1);
}

void delSem() {
  int rv = semctl(sem_id, 0, IPC_RMID);
  checkNeg(rv, "Error semctl");
}

void semBlock(int t){
  down();
  sleep(t);
  up();
}
