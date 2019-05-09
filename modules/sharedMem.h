#ifndef _MAINTENANCEMODULE_H_
#define _MAINTENANCEMODULE_H_
#include "../types/types.h"


void initShm();
void sshmdt();
void delShm();
void initArray();
int getSize();
int addProgram(Program p);
Program getProgram(int index);
void initSem(int val);
void getSem();
void addSem(int val);
void down();
void up();
void delSem();

#endif
