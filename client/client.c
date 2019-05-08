#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "../utils/utils.h"
#include "../utils/check.h"

#define MAX_INPUT 255
#define READ_SIZE 150
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
}

void timer_handler(){
  printf("Je suis minuteur\n");
}

int main(int argc, char const *argv[]) {
	char input[MAX_INPUT];
	int ret;
	int sockfd = initSocketClient(SERVER_IP, SERVER_PORT);
  printf("Veuillez entrez une commande : \n");
	ret = read(0, &input, MAX_INPUT);
	checkNeg(ret, "read client error");
	input[ret-1] = '\0';
	char command = input[0];
	switch (command) {
		case '+':{
			int commandToSend = -1;
			ret = write(sockfd, &commandToSend, sizeof(int));
			checkNeg(ret, "write client error");
			int fileNameLength = strlen(input+2);
			ret = write(sockfd, &fileNameLength, sizeof(int));
			checkNeg(ret, "write client error");
			ret = write(sockfd, input+2, fileNameLength*sizeof(char));
			checkNeg(ret, "write client error");
			int fd = open(input+2, O_RDONLY | O_CREAT, 0644);
			checkNeg(fd, "file descriptor client error");
			char buffer[READ_SIZE];
			int readChar;
			while ((readChar = read(fd, &buffer, READ_SIZE*sizeof(char))) != EOF){
				ret = write(sockfd, &buffer, readChar*sizeof(char));
				checkNeg(ret, "write client error");

			}
			close(fd);
			shutdown(sockfd, SHUT_WR);
		 	break;
		}
	 	default:
			printf("ya pas encore\n");
	}
	close(sockfd);

	// int ret = 15;
	//
	// int sockfd = initSocketClient(SERVER_IP, SERVER_PORT);
	//
	// write(sockfd, &ret, sizeof(int));
  // printf("Envoyé !\n");
	// checkNeg(ret,"write client error");
}
