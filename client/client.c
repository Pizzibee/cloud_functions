#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

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

void sendExecToServer(int programNumber);
void recurrentChild();
void timerChild(void* delay);
int extractFileNameIndex(char* input, int fileNameLength);
void sendAddToServer(char* input);
void readAddFromServer(int sockfd);
void readExecFromServer(int sockfd);
void executeProgramRecurrent(int programNumber);
void killChildRecurrent();
void killChildTimer();

int main(int argc, char *argv[]) {
  if (argc != 4){
    printf("Mauvais Format\n");
    exit(10);
  }
  //variable initialization
	char input[DEFAULT_SIZE];
	int ret;
  int delay;
  int recurrentChildPid;
  int timerChildPid;
  //argument extraction
  adr = argv[1];
  port = atoi(argv[2]);
  delay = atoi(argv[3]);
  //pipe creation
  spipe(pipefd);
  //childs execution
  recurrentChildPid = fork_and_run(&recurrentChild);
  timerChildPid = fork_and_run1(&timerChild, &delay);
  //pipe configuration
  sclose(pipefd[0]);
	while(true){
    //read on stdin
    printf("Veuillez entrez une commande : \n");
  	ret = read(0, &input, DEFAULT_SIZE);
    checkNeg(ret, "Error READ");
  	input[ret-1] = '\0';
  	char command = input[0];
  	switch (command) {
  		case '+':{
        //send command to server to add program
        sendAddToServer(input);
  		 	break;
  		}
  		case '@':{
        //send command to server to execute program
        int programNumber = atoi(&input[2]);
        sendExecToServer(programNumber);
  			break;
  		}
      case '*':{
        // send program number to recurrent child
        int programNumber = atoi(&input[2]);
        executeProgramRecurrent(programNumber);
        break;
      }
      case 'q':{
        sclose(pipefd[1]);
        kill(recurrentChildPid, SIGQUIT);
        kill(timerChildPid, SIGQUIT);
        exit(0);
        //kill childrens
      }
	  }
  }
}



void recurrentChild(){
  signal(SIGQUIT, killChildRecurrent);
  int tab[MAX_PARA_PROGRAM];
  int logicalSize = 0;
  sclose(pipefd[1]);
  while (true) {
    Message messageReceived;
    //read pipe
    read(pipefd[0], &messageReceived, sizeof(Message));
    if (messageReceived.type == 1){
      //if from pere then add to tab
      tab[logicalSize] = messageReceived.programNumber;
      logicalSize++;
    }else{
      //else from brother then iterate through tab and send exec commands to server
      for (int i = 0; i < logicalSize; i++) {
        sendExecToServer(tab[i]);
      }
    }
  }
}

void killChildRecurrent(){
  sclose(pipefd[0]);
  exit(0);
}

void killChildTimer(){
  sclose(pipefd[1]);
  exit(0);
}

void timerChild(void* delay){
  signal(SIGQUIT, killChildTimer);
  int* delayPtr = delay;
  int delayInt = *delayPtr;
  sclose(pipefd[0]);
  while (true) {
    sleep(delayInt);
    Message message;
    message.type = 2;
    write(pipefd[1], &message, sizeof(Message));
  }
}

int extractFileNameIndex(char* input, int fileNameLength){
  int fileNameIndex = -1;
  for (int i = fileNameLength; i >= 0; i--) {
    if (input[i] == '/'){
      fileNameIndex = i+1;
      break;
    }
  }
  return fileNameIndex;
}

void sendAddToServer(char* input){
  printf("----------------AJOUT PROGRAMME-------------\n");
  int sockfd = initSocketClient(adr, port);
  int commandToSend = -1;
  int fileNameLength = strlen(input+2);
  int fileNameIndex = extractFileNameIndex(input, fileNameLength);
  swrite(sockfd, &commandToSend, sizeof(int));
  swrite(sockfd, &fileNameLength, sizeof(int));
  if (fileNameIndex == -1){
    swrite(sockfd, input+2, fileNameLength*sizeof(char));
  }else{
    swrite(sockfd, input+fileNameIndex, fileNameLength*sizeof(char));
  }
  int fd = sopen(input+2, O_RDONLY | O_CREAT, 0644);
  char buffer[READ_SIZE];
  int readChar;
  while  ((readChar = sread(fd, buffer, READ_SIZE*sizeof(char))) != 0 ){
    swrite(sockfd, &buffer, readChar*sizeof(char));
  }
  sclose(fd);
  shutdown(sockfd, SHUT_WR);
  readAddFromServer(sockfd);
  sclose(sockfd);
  printf("--------------------------------------------\n\n");
}
void readAddFromServer(int sockfd){
  int programNumber;
  char buffer2[READ_SIZE];
  int readChar2;
  sread(sockfd, &programNumber, sizeof(int));
  printf("ID du programme ajouté : %d\n", programNumber);
  printf("Message d'erreur du compilateur :\n");
  while  ((readChar2 = sread(sockfd, buffer2, READ_SIZE*sizeof(char))) != 0){
    printf("%s\n", buffer2);
  }
}


void sendExecToServer(int programNumber){
  printf("----------------EXECUTION PROGRAMME-------------\n");
  int sockfd = initSocketClient(adr, port);
  int commandToSend = -2;
  swrite(sockfd, &commandToSend, sizeof(int));
  swrite(sockfd, &programNumber, sizeof(int));
  //read response from server
  readExecFromServer(sockfd);
  printf("--------------------------------------------\n\n");
  sclose(sockfd);
}

void readExecFromServer(int sockfd){
  int programNumberReceived;
  int programState;
  sread(sockfd, &programNumberReceived, sizeof(int));
  printf("ID du programme executé : %d\n", programNumberReceived);
  sread(sockfd, &programState, sizeof(int));
  switch (programState) {
    case -2:{
      printf("Le programme n°%d n'existe pas.\n", programNumberReceived);
      break;
    }
    case -1:{
      printf("Le programme n°%d ne compile pas.\n", programNumberReceived);
      break;
    }
    case -0:{
      printf("Le programme n°%d ne s'est pas terminé normalement.\n", programNumberReceived);
      break;
    }
    case 1:{
      printf("Le programme n°%d s'est terminé normalement.\n", programNumberReceived);
      long executionTime;
      int returnCode;
      sread(sockfd, &executionTime, sizeof(long));
      printf("Le temps d'exécution du programme n°%d : %ld microsecondes\n", programNumberReceived, executionTime);
      sread(sockfd, &returnCode, sizeof(int));
      printf("Le code de retour du programme n°%d : %d\n", programNumberReceived, returnCode);
      char buffer[READ_SIZE];
      int readChar;
      printf("Affichage à la sortie standard du programme n°%d :\n", programNumberReceived);
      while  ((readChar = sread(sockfd, buffer, READ_SIZE*sizeof(char))) != 0 ){
        printf("%s\n", buffer);
      }
      break;
    }
  }
}

void executeProgramRecurrent(int programNumber){
  Message message;
  message.type = 1;
  message.programNumber = programNumber;
  write(pipefd[1], &message, sizeof(Message));
}
