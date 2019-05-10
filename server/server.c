#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include "../types/types.h"
#include "../modules/sharedMem.h"
#include "../modules/utils.h"
#include "../modules/check.h"
#include "../modules/socket.h"

#define SERVER_PORT 9500
#define SERVER_IP	"127.0.0.1"  /* localhost */
#define MAX_SIZE 255
#define MAX_BUF 150

int compileC(Program p);
long now();
void addProg(void*);
void exec1(void* path);
void exec2(void* path);


int compileC(Program p){
  int ret;
  char pathCompile[MAX_SIZE];
  strcpy(pathCompile,p.sourceFile);
  pathCompile[strlen(pathCompile)-2] = '\0';
  strcat(pathCompile, "_res_compile.txt");
  int fdCompile = open(pathCompile, O_CREAT | O_WRONLY| O_TRUNC, 0666);
  checkNeg(fdCompile, "ERROR open");
  int stderr_copy = dup(2);
  checkNeg(stderr_copy, "ERROR dup");
  ret = dup2(fdCompile, 2);
  checkNeg(ret, "ERROR dup2");
  fork_and_run1(&exec1,&p.sourceFile);
  int status;
  wait(&status);
  ret = dup2(stderr_copy, 2);
  checkNeg(ret, "ERROR dup");
  close(stderr_copy);
  close(fdCompile);
  return status;
}

long now() {
  struct timeval  tv;

  int res = gettimeofday(&tv, NULL);
  checkNeg(res, "Error gettimeofday");

  return tv.tv_sec * 1000000 + tv.tv_usec;
}

void exec1(void* path) {
  char* pathProg = path;
  char pathFolder[MAX_SIZE];
  strcpy(pathFolder,pathProg);
  pathFolder[strlen(pathProg)-2] = '\0';
  execl("/usr/bin/gcc", "gcc", "-o", pathFolder, pathProg, NULL);
  perror("Error execl 1");
}

void exec2(void* path) {
  char* pathProg = path;
  char pathFolder[MAX_SIZE];
  strcpy(pathFolder,pathProg);
  pathFolder[strlen(pathProg)-2] = '\0';
  execl(pathFolder, NULL);
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
    p.executionCounter = 0;
    p.executionTime = 0;
    int id = addProgram(p);
    ret = write(newsockfd,&id,sizeof(int));
    checkNeg(ret,"server write id size error");
    int status = compileC(p);

    if (WIFEXITED(status) && !WEXITSTATUS(status)) {
      p.compilationError = false;
      char compilRes[MAX_SIZE];
      strcpy(compilRes,"compilation ok");
      ret = write(newsockfd,&compilRes,strlen(compilRes)*sizeof(char));
      checkNeg(ret,"server write id size error");
    } else {
      p.compilationError = true;
      char bufErr[MAX_BUF];
      path[strlen(path)-2] = '\0';
      strcat(path, "_res_compile.txt");
      int fdErr = open(path, O_RDONLY | O_CREAT, 0666);
      checkNeg(fdErr,"file descriptor error");
      while((readChar = read(fdErr,bufErr,MAX_BUF*sizeof(char))) != 0){
        printf("%s\n",bufErr );
        ret = write(newsockfd, bufErr, readChar*sizeof(char));
        checkNeg(ret,"server write file error");
      }
      close(fdErr);
    }


    // EXECUTE A PROGRAM
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
    else{
      p = getProgram(programId);
      int status = compileC(p);

      if (WIFEXITED(status) && !WEXITSTATUS(status)) {
        p.compilationError = false;
        char pathRes[MAX_SIZE];
        strcpy(pathRes,p.sourceFile);
        pathRes[strlen(pathRes)-2] = '\0';
        strcat(pathRes, "_res_execution.txt");
        int fdExec = open(pathRes, O_CREAT | O_WRONLY| O_TRUNC, 0666);
        checkNeg(fdExec, "ERROR open");
        int stdout = dup(1);
        checkNeg(stdout, "ERROR dup");
        ret = dup2(fdExec, 1);
        checkNeg(ret, "ERROR dup2");
        long t1 = now();
        fork_and_run1(&exec2,&p.sourceFile);
        wait(&status);
        ret = dup2(stdout, 1);
        checkNeg(ret, "ERROR dup");
        close(stdout);
        close(fdExec);
        long t2 = now();
        long execTime = t2 - t1;
        p.executionCounter += 1;
        p.executionTime += execTime;
        int progStatus = WEXITSTATUS(status);
        if(WIFEXITED(status) != 0){
          int req = 1;
          ret = write(newsockfd,&req,sizeof(int));
          checkNeg(ret,"server write id size error");
          ret = write(newsockfd,&execTime,sizeof(long));
          checkNeg(ret,"server write id size error");
          ret = write(newsockfd,&progStatus,sizeof(int));
          checkNeg(ret,"server write id size error");
          int fd = open(pathRes, O_RDONLY | O_CREAT, 0666);
          checkNeg(fd, "file descriptor client error");
          char buffer[MAX_SIZE];
          int readChar;
          while  ((readChar = read(fd, buffer, MAX_SIZE*sizeof(char))) != 0 ){
            printf("%s\n", buffer);
            ret = write(newsockfd, &buffer, readChar*sizeof(char));
            checkNeg(ret, "write client error");
          }
        }
        else{
          int req = 0;
          ret = write(newsockfd,&req,sizeof(int));
          checkNeg(ret,"server write id size error");
        }
      }
      else{
        p.compilationError = true;
        int req = -1;
        ret = write(newsockfd,&req,sizeof(int));
        checkNeg(ret,"server write id size error");
      }
    }
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
