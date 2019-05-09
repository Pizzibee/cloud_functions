#ifndef _CHECK_H_
#define _CHECK_H_

#include <stdbool.h>

void checkCond(bool cond, char *msg);

void checkNeg(int res, char *msg);

void checkNull(void *res, char *msg);

#endif
