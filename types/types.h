#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdbool.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#define FILE_SIZE 255
#define MAX_PROGRAM 1000

typedef struct{
  int id;
  char sourceFile[FILE_SIZE];
  bool compilationError;
  int executionCounter;
  int executionTime;
} Program;

typedef struct{
  Program tabProg[MAX_PROGRAM];
  int size;
} ProgramArray;

typedef struct{
  int type; //from son or father
  int programNumber; 
} Message;

#endif
