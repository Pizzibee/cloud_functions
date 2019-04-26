#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdbool.h>
#define FILE_SIZE 255
#define SIZE_MAX_PROGRAM 1000

typedef struct{
  int id;
  char sourceFile[FILE_SIZE];
  bool compilationError;
  int executionCounter;
  int executionTime;
} Program;

typedef struct{
  Program tabProg[SIZE_MAX_PROGRAM];
  int logicalSize;
  int physicalSize;
} ProgramArray;

#endif
