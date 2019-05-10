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
char *adr;
int port;

void sendExecToServer(int programNumber){
  int sockfd = initSocketClient(adr, port);
  int commandToSend = -2;
  int ret;
  ret = write(sockfd, &commandToSend, sizeof(int));
  checkNeg(ret, "write client error");
  ret = write(sockfd, &programNumber, sizeof(int));
  checkNeg(ret, "write client error");
  //read response from server
  int programNumberReceived;
  int programState;
  ret = read(sockfd, &programNumberReceived, sizeof(int));
  checkNeg(ret, "read client error");
  printf("Id du programme executé : %d\n", programNumberReceived);
  ret = read(sockfd, &programState, sizeof(int));
  checkNeg(ret, "read client error");
  printf("etat prog : %d\n", programState);
  switch (programState) {
    case -2:{
      printf("Le programme %d n'existe pas\n", programNumberReceived);
      break;
    }
    case -1:{
      printf("Le programme %d ne compile pas\n", programNumberReceived);
      break;
    }
    case -0:{
      printf("Le programme %d ne s'est pas terminé normalement\n", programNumberReceived);
      break;
    }
    case 1:{
      printf("Le programme %d s'est terminé normalement\n", programNumberReceived);
      long executionTime;
      int returnCode;
      ret = read(sockfd, &executionTime, sizeof(long));
      checkNeg(ret, "read client error");
      printf("Le temps d'exécution du programme : %ld microsecondes\n", executionTime);
      ret = read(sockfd, &returnCode, sizeof(int));
      checkNeg(ret, "read client error");
      printf("Le code de retour du programme : %d\n", returnCode);
      char buffer[READ_SIZE];
      int readChar;
      printf("Afficha à la sortie standard du programme %d :\n", programNumberReceived);
      while  ((readChar = read(sockfd, buffer, READ_SIZE*sizeof(char))) != 0 ){
        printf("%s\n", buffer);
      }
      break;
    }
    default:{
      printf("etat du prog reçu %d\n", programState);
      break;
    }
  }
  close(sockfd);
}

void recurrent_handler(void* pipe0, void* pipe1){
  int tab[MAX_PARA_PROGRAM];
  int logicalSize = 0;
  int ret;
  ret = close(pipefd[1]);
  checkNeg(ret, "close pipe client error recurrent function");
  while (true) {
    Message messageReceived;
    //read pipe
    read(pipefd[0], &messageReceived, sizeof(Message));
    printf("Type received : %d\n", messageReceived.type);
    if (messageReceived.type == 1){
      //if from pere then add to tab
      printf("ProgramNumber received %d\n", messageReceived.programNumber);
      tab[logicalSize] = messageReceived.programNumber;
      logicalSize++;
    }else{
      //else from brother then iterate through tab and send exec commands to server
      for (int i = 0; i < logicalSize; i++) {
        sendExecToServer(tab[i]);
      }
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
  while (true) {
    sleep(delayInt);
    Message message;
    message.type = 2;
    write(pipefd[1], &message, sizeof(Message));
  }
  close(pipefd[1]);
}


int main(int argc, char *argv[]) {
  if (argc != 4){
    printf("Mauvais Format\n");
    exit(10);
  }
	char input[DEFAULT_SIZE];
	int ret;
  adr = argv[1];
  port = atoi(argv[2]);
  int delay = atoi(argv[3]);
  ret = pipe(pipefd);
  checkNeg(ret, "create pipe client error");
  fork_and_run2(&recurrent_handler, &pipefd[0], &pipefd[1]);
  fork_and_run1(&timer_handler, &delay);
  ret = close(pipefd[0]);
  checkNeg(ret, "close pipe client error main");

	while(true){
    //read on stdin
    printf("Veuillez entrez une commande : \n");
  	ret = read(0, &input, DEFAULT_SIZE);
  	checkNeg(ret, "read client error");
  	input[ret-1] = '\0';
    //extract command + * @ q
  	char command = input[0];
  	switch (command) {
  		case '+':{
        //send command to server to add program
        int sockfd = initSocketClient(adr, port);
  			int commandToSend = -1;
  			int fileNameLength = strlen(input+2);
  			ret = write(sockfd, &commandToSend, sizeof(int));
  			checkNeg(ret, "write client error");
  			ret = write(sockfd, &fileNameLength, sizeof(int));
  			checkNeg(ret, "write client error");
        //extract filename from path entered
  			int fileNameIndex = -1;
  			for (int i = fileNameLength; i >= 0; i--) {
  				if (input[i] == '/'){
  					fileNameIndex = i+1;
  					break;
  				}
  			}
  			if (fileNameIndex == -1){
          //if only file name entered
  				ret = write(sockfd, input+2, fileNameLength*sizeof(char));
  			}else{
          //if path entered
  				ret = write(sockfd, input+fileNameIndex, fileNameLength*sizeof(char));
  			}
  			checkNeg(ret, "write client error");
        //open and reads the file and write its content to socket
  			int fd = open(input+2, O_RDONLY | O_CREAT, 0644);
  			checkNeg(fd, "open client error");
  			char buffer[READ_SIZE];
  			int readChar;
  			while  ((readChar = read(fd, buffer, READ_SIZE*sizeof(char))) != 0 ){
  				ret = write(sockfd, &buffer, readChar*sizeof(char));
  				checkNeg(ret, "write client error");
  			}
  			close(fd);
  			shutdown(sockfd, SHUT_WR);
        //reads the response from the server and displays it to stdout
  			int programNumber;
        char buffer2[READ_SIZE];
  			int readChar2;
  			ret = read(sockfd, &programNumber, sizeof(int));
  			checkNeg(ret, "read client error");
  			printf("Id du programme ajouté : %d\n", programNumber);
  			printf("Message d'erreur du compilateur :\n");
  			while  ((readChar2 = read(sockfd, buffer2, READ_SIZE*sizeof(char))) != 0 ){
  				printf("%s\n", buffer2);
  			}
      	close(sockfd);
  		 	break;
  		}
  		case '@':{
        //send command to server to execute program
        int programNumber = atoi(&input[2]);
        sendExecToServer(programNumber);
  			break;
  		}
      case '*':{
        // send program number to recurrent child in struct Message with type 1 (pipe)
        int programNumber = atoi(&input[2]);
        Message message;
        message.type = 1;
        message.programNumber = programNumber;
        write(pipefd[1], &message, sizeof(Message));
        break;
      }
      case 'q':{
        exit(1);
        //quit server and free ressources
      }
	  }
  }
  close(pipefd[1]);
}
