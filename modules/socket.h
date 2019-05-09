#ifndef _SOCKET_H_
#define _SOCKET_H_


#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>


//*****************************************************************************
// RESEAU
//*****************************************************************************
/* Convert a DNS domain name to IP v4
 Necessary for socket connection because IP required */
int hostnameToIp(char *hostname, char *ip);
int initSocketClient(char ServerIP[16], int Serverport);
int initSocketServer(int port);
#endif
