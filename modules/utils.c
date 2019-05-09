#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <signal.h>

#include "utils.h"
#include "check.h"

//*****************************************************************************
// FILE
//*****************************************************************************
int sopen(char* path, int flag, int mode) {
  int fd = open(path, flag, mode);
  checkNeg(fd, "Error OPEN");
  return fd;
}

int sopen1(char* path) {
  return sopen(path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
}


void swrite(int fd, void* buff, int size) {
  int r = write(fd, buff, size);
  checkCond(r != size, "Error WRITE");
}

int sread(int fd, void* buff, int size) {
  int r = read(fd, buff, size);
  checkNeg(r, "Error READ");
  return r;
}

// READ EXACTLY n chars
int nread(int fd, void* buff, int n) {
  char* cbuff = (char*) buff;
  int s = sread(fd, cbuff, n);
  int i = s;
  while(s != 0 && i != n) {
    i += s;
    s =  sread(fd, cbuff + i, n - i);
  }
  return s;
}

void sclose(int fd) {
  int r = close(fd);
  checkNeg(r, "Error CLOSE");
}

//*****************************************************************************
// FORK
//*****************************************************************************

pid_t fork_and_run(void (*handler)()) {
  int childId = fork();
  checkNeg(childId, "Error [fork_and_run]");

  // child process
  if (childId == 0) {
    (*handler)();
    exit(EXIT_SUCCESS);
  }

  return childId;
}

pid_t fork_and_run1(void (*handler)(void *), void* arg0) {
  int childId = fork();
  checkNeg(childId, "Error [fork_and_run]");

  // child process
  if (childId == 0) {
    (*handler)(arg0);
    exit(EXIT_SUCCESS);
  }

  return childId;
}

pid_t fork_and_run2(void (*handler)(void*, void*), void* arg0, void* arg1) {
  int childId = fork();
  checkNeg(childId, "Error [fork_and_run]");

  // child process
  if (childId == 0) {
    (*handler)(arg0, arg1);
    exit(EXIT_SUCCESS);
  }

  return childId;
}

void swaitpid(pid_t pid, int* status) {
  pid_t waitId = waitpid(pid, status, 0);
  checkNeg(waitId, "Error waitpid()");
}

//*****************************************************************************
// PIPE
//*****************************************************************************
void spipe(int* fildes) {
  int r = pipe(fildes);
  checkNeg(r, "Error pipe");
}

//*****************************************************************************
// SIGNAL
//*****************************************************************************
void ssigaction(int signum, void (*handler)(int)) {
  struct sigaction action;
  action.sa_handler = handler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;

  int r = sigaction (signum, &action, NULL);
  checkNeg(r, "Error sigaction");
}



/*
        buffered read
*/
int readline(int fd, char *line) {
  static int nbChar;
  static char buffer[BUFFER_SIZE];
  static int i = 0;
  int j = 0;

  while (1) {
    if ((i == 0) || (i == BUFFER_SIZE)) {
      nbChar = read(fd, buffer, BUFFER_SIZE);
      i = 0;
      if (nbChar == 0)
        return 0;
    }

    if (i >= (nbChar - 1))
      return 0;

    while (i < BUFFER_SIZE) {
      if (buffer[i] == '\n') {
        /* return line is a string */
        line[j] = '\0';
        i++; // pass \n for next
        // printf("i : %i\n",i);

        return 1;
      } else {

        line[j] = buffer[i];
        i++;
        j++;
      }
    }
  }
}
