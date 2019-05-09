#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "../types/types.h"
#include "../modules/sharedMem.h"
#include "../modules/utils.h"
#include "../modules/check.h"
#include "../modules/socket.h"

#define SERVER_PORT 9500
#define SERVER_IP	"127.0.0.1"  /* localhost */
#define MAX_SIZE 255
#define MAX_BUF 150

void addProg(void*);
void exec1();
void exec2();

void exec1() {
  execl("/usr/bin/gcc", "gcc", "-o", "serverPrograms/hello", "serverPrograms/hello.c", NULL);
  perror("Error execl 1");
}

void exec2() {
  execl("./hello", "serverPrograms/hello", NULL);
  perror("Error exec 2");
}


void addProg(void* sock){
  int* sockInt = sock;
  int newsockfd = *sockInt;
  int req, ret;
  Program p;
  ret = read(newsockfd,&req,sizeof(int));
  checkNeg(ret,"server read choice error");
  if (req == -1) {
    int sizeName;
    char fileName[MAX_SIZE];
    char path[MAX_SIZE] = "serverPrograms/";
    char buf[MAX_BUF];
    int fd;
    int readChar;
    ret = read(newsockfd,&sizeName,sizeof(int));
    checkNeg(ret,"server read file size error");
    printf("%d\n",sizeName);
    ret = read(newsockfd,&fileName,sizeName*sizeof(char));
    checkNeg(ret,"server read file size error");
    printf("%s\n",fileName);
    strcat(path, fileName);
    printf("%s\n", path);
    fd = open(path, O_RDWR | O_TRUNC | O_CREAT, 0644);
    checkNeg(fd,"file descriptor error");
    while((readChar = read(newsockfd,buf,MAX_BUF*sizeof(char))) != 0){
      ret = write(fd, buf, readChar*sizeof(char));
      checkNeg(ret,"server write file error");
    }
    close(fd);
    strcpy(p.sourceFile,path);
    p.executionCounter = 1;
    int id = addProgram(p);
    ret = write(newsockfd,&id,sizeof(int));
    checkNeg(ret,"server write id size error");
    ret = write(newsockfd,&buf,MAX_BUF*sizeof(char));
  } else{
    int programId;
    ret = read(newsockfd,&programId,sizeof(int));
    checkNeg(ret,"server read choice error");
    printf("%d\n", programId);
    ret = write(newsockfd,&programId,sizeof(int));
    checkNeg(ret,"server write id size error");
    if(programId >= getSize()){
      int req = -2;
      ret = write(newsockfd,&req,sizeof(int));
      checkNeg(ret,"server write id size error");
    }

    p = getProgram(programId);
    char pathCompile[MAX_SIZE];
    strcpy(pathCompile,p.sourceFile);
    strcat(pathCompile, "_res_compile.txt");
    int fdCompile = open(pathCompile, O_CREAT | O_WRONLY| O_TRUNC, 0666);
    checkNeg(fdCompile, "ERROR open");
    int stderr_copy = dup(2);
    checkNeg(stderr_copy, "ERROR dup");
    ret = dup2(fdCompile, 2);
    checkNeg(ret, "ERROR dup2");
    fork_and_run(exec1);
    int status;
    wait(&status);
    ret = dup2(stderr_copy, 2);
    checkNeg(ret, "ERROR dup");
    close(stderr_copy);
  }
  close(newsockfd);
}



int main(int argc, char const *argv[]) {
  printf("Server hello\n");
  int sockfd, newsockfd;
  getSem();
  initShm();
  sockfd = initSocketServer(SERVER_PORT);
  printf("Le serveur tourne sur le port : %i \n",SERVER_PORT);
  while (1) {
    newsockfd = accept(sockfd, NULL,NULL);
    if (newsockfd > 0 ) {
      fork_and_run1(&addProg, &newsockfd);
    }
  }
  sshmdt();
  return 0;
}
