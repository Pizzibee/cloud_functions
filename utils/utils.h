#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdbool.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

#define SERVER_PORT 		9500
#define SERVER_IP		"127.0.0.1"  /* localhost */

//*****************************************************************************
// FILE
//*****************************************************************************
int sopen1(char* path);

int sopen(char* path, int flag, int mode);

void swrite(int fd, void* buff, int size);

int sread(int fd, void* buff, int size);

int nread(int fd, void* buff, int n);

void sclose(int fd);

//*****************************************************************************
// FORK
//*****************************************************************************

pid_t fork_and_run(void (*run)());

pid_t fork_and_run1(void (*handler)(void *), void* arg0);

pid_t fork_and_run2(void (*handler)(void *, void *), void* arg0, void* arg1);

void swaitpid(pid_t pid, int* status);

//*****************************************************************************
// PIPE
//*****************************************************************************

void spipe(int* fildes);

//*****************************************************************************
// SIGNAL
//*****************************************************************************

void ssigaction(int signum, void (*handler)(int signum));



//*****************************************************************************
// RESEAU
//*****************************************************************************
/* Convert a DNS domain name to IP v4
 Necessary for socket connection because IP required */
int hostname_to_ip(char *hostname, char *ip);

/*
        buffered read
*/
int readline(int fd, char *line);

#endif
