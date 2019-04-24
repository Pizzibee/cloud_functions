#include <stdio.h>
#include <stdlib.h>


#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

#include "utils.h"

#define SHM_KEY 248
#define SEM_KEY 369
#define PERM 0666

//cf man semctl
// This function has three or four arguments, depending on cmd.  
// When there are four, the fourth has the type union semun.  
// The  calling program must define this union as follows:
union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};


//******************************************************************************
//GLOBAL VARIABLES
//******************************************************************************
int shm_id;
int sem_id;
int* z;

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

//******************************************************************************
//SEMAPHORE
//******************************************************************************
void init_sem(int val) {
  
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

void add_sem(int val) {
  struct sembuf sem; 
  sem.sem_num = 0; 
  sem.sem_op = val; 
  sem.sem_flg = 0;

  int rc = semop(sem_id, &sem, 1);
  checkNeg(rc, "Error semop");
}

void down() {
  add_sem(-1);
}

void up() {
  add_sem(1);
}

void del_sem() {
  int rv = semctl(sem_id, 0, IPC_RMID);
  checkNeg(rv, "Error semctl");
}

//******************************************************************************
//CHILD HANDLER
//******************************************************************************
void child_handler() {
  *z = 987654321;
  up();
  sshmdt();
}

//******************************************************************************
//MAIN FUNCTION
//******************************************************************************
int main (int argc, char *argv[]) {
  init_sem(0);
  init_shm();
  
  fork_and_run(&child_handler);
  
  down();
  printf("*z = %d\n", *z);
  
  sshmdt();
  
  // COMMENT OR UNCOMMENT
  //del_shm();
  //del_sem();
  return 0;
}