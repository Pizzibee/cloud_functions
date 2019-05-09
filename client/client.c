#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "../modules/utils.h"
#include "../modules/check.h"
#include "../modules/socket.h"
#include "../types/types.h"

#define DEFAULT_SIZE 255
#define READ_SIZE 150
#define MAX_PARA_PROGRAM 50

int pipefd[2];

void recurrent_handler(void* pipe0, void* pipe1){
  printf("Fils 1\n");
  int tab[MAX_PARA_PROGRAM];
  int logicalSize = 0;
  //read pipe
  //if from pere then add to tab
  //if from brother then iterate through tab and send exec commands to server
  int ret;
  ret = close(pipefd[1]);
  checkNeg(ret, "close pipe client error recurrent function");
  while (true) {
    Message messageReceived;
    read(pipefd[0], &messageReceived, sizeof(Message));
    printf("type received : %d\n", messageReceived.type);
    printf("programNumber received %d\n", messageReceived.programNumber);
    if (messageReceived.type == 1){
      tab[logicalSize] = messageReceived.programNumber;
      logicalSize++;
    }else{
      for (int i = 0; i < logicalSize; i++) {
        printf("%d\n", tab[i]);
      }
      printf("type 2 reccurent function done\n");
    }
  }
  close(pipefd[0]);
}

void timer_handler(void* delay){
  int* delayPtr = delay;
  int delayInt = *delayPtr;
  int ret;
  ret = close(pipefd[0]);
  checkNeg(ret, "close pipe client error timer");
  printf("Fils 2 %d\n", delayInt);
  while (true) {
    sleep(delayInt);
    Message message;
    message.type = 2;
    write(pipefd[1], &message, sizeof(Message));
  }
  close(pipefd[1]);
}

void sendExecToServer(int sockfd, int programNumber){
  int commandToSend = -2;
  int ret;
  ret = write(sockfd, &commandToSend, sizeof(int));
  checkNeg(ret, "write client error");
  ret = write(sockfd, &programNumber, sizeof(int));
  checkNeg(ret, "write client error");
}

int main(int argc, char *argv[]) {
  if (argc != 4){
    printf("Mauvais Format\n");
    exit(10);
  }
	char input[DEFAULT_SIZE];
	int ret;
  char *adr = argv[1];
  int port = atoi(argv[2]);
  int delay = atoi(argv[3]);
  ret = pipe(pipefd);
  checkNeg(ret, "create pipe client error");
  fork_and_run2(&recurrent_handler, &pipefd[0], &pipefd[1]);
  fork_and_run1(&timer_handler, &delay);
  ret = close(pipefd[0]);
  checkNeg(ret, "close pipe client error main");

  //-- boucle while
	while(true){
    int sockfd = initSocketClient(adr, port);
    printf("Veuillez entrez une commande : \n");
  	ret = read(0, &input, DEFAULT_SIZE);
  	checkNeg(ret, "read client error");
  	input[ret-1] = '\0';
  	char command = input[0];
  	switch (command) {
  		case '+':{
        //send command to server to add program
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
  				ret = write(sockfd, &buffer, readChar*sizeof(char));
  				checkNeg(ret, "write client error");
  			}
  			close(fd);
  			shutdown(sockfd, SHUT_WR);
  			int programNumber;
  			ret = read(sockfd, &programNumber, sizeof(int));
  			checkNeg(ret, "read client programNumber error");
  			printf("Id du programme ajouté : %d\n", programNumber);
  			printf("Message d'erreur du compilateur :\n");
        char buffer2[READ_SIZE];
  			int readChar2;
  			while  ((readChar2 = read(sockfd, buffer2, READ_SIZE*sizeof(char))) != 0 ){
  				printf("%s\n", buffer2);
  			}
  		 	break;
  		}
  		case '@':{
        //send command to server to execute program
        int programNumber = atoi(&input[2]);
        sendExecToServer(sockfd, programNumber);
        //read les reponses du serveur
  			break;
  		}
      case '*':{
        // send program number to recurrent child in struct Message with type 1 (pipe)
        int programNumber = atoi(&input[2]);
        Message message;
        message.type = 1;
        message.programNumber = programNumber;
        printf("bklablabal\n");
        write(pipefd[1], &message, sizeof(Message));
        break;
      }
      case 'q':{
        //quit server and free ressources
      }
  	 	default:
  			printf("ya pas encore\n");
	}
  	close(sockfd);
}
  //fin boucle while
  close(pipefd[1]);
}
