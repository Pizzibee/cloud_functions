#include <stdio.h>
#include <stdlib.h>

#include "../utils/utils.h"
#include "../utils/check.h"
struct sockaddr_in addr;
int initSocketClient(char ServerIP[16], int Serverport){
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
void prompt_handler(){
  printf("Je suis le prompt\n");
}

void timer_handler(){
  printf("Je suis minuteur\n");
}

int main(int argc, char const *argv[]) {
	int ret = 15;

	int sockfd = initSocketClient(SERVER_IP, SERVER_PORT);

	ret = write(sockfd, &ret, sizeof(int));
  printf("Envoyé !\n");
	checkNeg(ret,"write client error");
}
