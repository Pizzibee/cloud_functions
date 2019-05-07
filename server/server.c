#include <stdio.h>
#include <stdlib.h>
#include "../types/types.h"
#include "../utils/sharedMem.h"
#include "../utils/utils.h"
#include "../utils/check.h"

struct sockaddr_in addr;
/* return sockfd */
int initSocketServer(int port)
{
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

int main(int argc, char const *argv[]) {
  printf("Server hello\n");
  int sockfd, newsockfd, i, ret;

  initShm();
  sockfd = initSocketServer(SERVER_PORT);
  printf("Le serveur tourne sur le port : %i \n",SERVER_PORT);
  while (1) {
    /* client trt */
    newsockfd = accept(sockfd, NULL,NULL);
    if (newsockfd > 0 ) {
      ret = read(newsockfd,&i,sizeof(int));
      checkNeg(ret,"server read error");
      printf("j'ai reçu %d\n",i );
    }
  }
  sshmdt();
  return 0;
}
