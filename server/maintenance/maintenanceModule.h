#ifndef _MAINTENANCEMODULE_H_
#define _MAINTENANCEMODULE_H_
#include "../../types/types.h"

void init_shm();
void sshmdt();
void del_shm();
void init_array();
Program* get_array();
int get_size();
void add_program(Program p);

#endif
