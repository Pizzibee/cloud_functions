#ifndef _UTILS_H_
#define _UTILS_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

/* Convert a DNS domain name to IP v4
 Necessary for socket connection because IP required */
int hostname_to_ip(char *hostname, char *ip);

/*
        buffered read
*/
int readline(int fd, char *line);

#endif
