#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "../modules/utils.h"
#include "../modules/check.h"
#include "../modules/socket.h"

#define DEFAULT_SIZE 255
#define READ_SIZE 150

#define SERVER_PORT 9500
#define SERVER_IP	"127.0.0.1"  /* localhost */

void recurrent_handler(){
}

void timer_handler(){
  printf("Je suis minuteur\n");
}

int main(int argc, char const *argv[]) {
	char input[DEFAULT_SIZE];
	int ret;
	int sockfd = initSocketClient(SERVER_IP, SERVER_PORT);
  printf("Veuillez entrez une commande : \n");
	ret = read(0, &input, DEFAULT_SIZE);
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
			int fileNameIndex = -1;
			for (int i = fileNameLength; i >= 0; i--) {
				if (input[i] == '/'){
					fileNameIndex = i+1;
					break;
				}
			}
			if (fileNameIndex == -1){
				ret = write(sockfd, input+2, fileNameLength*sizeof(char));
			}else{
				ret = write(sockfd, input+fileNameIndex, fileNameLength*sizeof(char));
			}
			checkNeg(ret, "write client error");
			int fd = open(input+2, O_RDONLY | O_CREAT, 0644);
			checkNeg(fd, "file descriptor client error");
			char buffer[READ_SIZE];
			int readChar;
			while  ((readChar = read(fd, buffer, READ_SIZE*sizeof(char))) != 0 ){
				printf("%s\n", buffer);
				ret = write(sockfd, &buffer, readChar*sizeof(char));
				checkNeg(ret, "write client error");
			}
			close(fd);
			shutdown(sockfd, SHUT_WR);
			int programNumber;
			char errorMessage[DEFAULT_SIZE];
			ret = read(sockfd, &programNumber, sizeof(int));
			checkNeg(ret, "read client programNumber error");
			ret = read(sockfd, &errorMessage, READ_SIZE*sizeof(char));
			checkNeg(ret, "read client errorMessage error");
			printf("Id du programme ajouté : %d\n", programNumber);
			//TODO : changer en une bouclie while
			printf("Message d'erreur du compilateur : %s\n", errorMessage);
		 	break;
		}
		case '@':{
			int commandToSend = -2;
			ret = write(sockfd, &commandToSend, sizeof(int));
			checkNeg(ret, "write client error");
			int programNumber = atoi(&input[2]);
			ret = write(sockfd, &programNumber, sizeof(int));
			checkNeg(ret, "write client error");
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
