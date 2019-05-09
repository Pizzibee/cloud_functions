

#include "socket.h"

//*****************************************************************************
// RESEAU
//*****************************************************************************
/* Convert a DNS domain name to IP v4
 Necessary for socket connection because IP required */
int hostnameToIp(char *hostname, char *ip) {
  struct hostent *he;
  struct in_addr **addr_list;
  int i;

  if ((he = gethostbyname(hostname)) == NULL) {
    herror("gethostbyname");
    return 1;
  }

  addr_list = (struct in_addr **)he->h_addr_list;

  for (i = 0; addr_list[i] != NULL; i++) {
    strcpy(ip, inet_ntoa(*addr_list[i]));
    return 0;
  }
  return 1;
}

int initSocketClient(char ServerIP[16], int Serverport){
  struct sockaddr_in addr;
	// socket creation
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	// prepare socket to connect
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(Serverport);
	inet_aton(ServerIP,&addr.sin_addr);
	connect(sockfd, (struct sockaddr *) &addr,sizeof(addr));
	return sockfd;
}

/* return sockfd */
int initSocketServer(int port){
  struct sockaddr_in addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	memset(&addr,0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	// listen on all server interfaces
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sockfd,(struct sockaddr*)&addr,sizeof(addr));
	listen(sockfd,5);
	return sockfd;
}
