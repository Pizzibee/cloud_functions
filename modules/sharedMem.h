#ifndef _MAINTENANCEMODULE_H_
#define _MAINTENANCEMODULE_H_
#include "../types/types.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void initShm();
void sshmdt();
void delShm();
void initArray();
int getSize();
int addProgram(Program p);
Program getProgram(int index);
void setProgram(int id, Program p);
void initSem(int val);
void getSem();
void addSem(int val);
void down();
void up();
void delSem();
void semBlock(int t);

#endif
