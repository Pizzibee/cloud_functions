
#include <stdbool.h>

typedef struct{
  int id;
  char* sourceFile;
  bool compilationError;
  int executionCounter;
  int executionTime;
} Program;
