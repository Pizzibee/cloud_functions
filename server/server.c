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
  int fdCompile = sopen(pathCompile, O_CREAT | O_WRONLY| O_TRUNC, 0666);
  int stderr_copy = dup(2);
  checkNeg(stderr_copy, "ERROR dup");
  ret = dup2(fdCompile, 2);
  checkNeg(ret, "ERROR dup2");
  fork_and_run1(&exec1,&p.sourceFile);
  int status;
  wait(&status);
  ret = dup2(stderr_copy, 2);
  checkNeg(ret, "ERROR dup");
  sclose(stderr_copy);
  sclose(fdCompile);
  return status;
}

long now() {
  struct timeval tv;

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
  sread(newsockfd,&req,sizeof(int));
  if (req == -1) {
    int sizeName;
    char fileName[MAX_SIZE];
    char path[MAX_SIZE] = "serverPrograms/";
    char buf[MAX_BUF];
    int fd;
    int readChar;
    sread(newsockfd,&sizeName,sizeof(int));
    sread(newsockfd,&fileName,sizeName*sizeof(char));
    strcat(path, fileName);
    fd = sopen(path, O_RDWR | O_TRUNC | O_CREAT, 0644);
    while((readChar = sread(newsockfd,buf,MAX_BUF*sizeof(char))) != 0){
      swrite(fd, buf, readChar*sizeof(char));
    }
    sclose(fd);
    strcpy(p.sourceFile,path);
    p.executionCounter = 0;
    p.executionTime = 0;
    int id = addProgram(p);
    swrite(newsockfd,&id,sizeof(int));
    int status = compileC(p);

    if (WIFEXITED(status) && !WEXITSTATUS(status)) {
      p.compilationError = false;
      char compilRes[MAX_BUF];
      strcpy(compilRes,"compilation ok");
      swrite(newsockfd,&compilRes,strlen(compilRes)*sizeof(char));
    } else {
      p.compilationError = true;
      char bufErr[MAX_BUF];
      path[strlen(path)-2] = '\0';
      strcat(path, "_res_compile.txt");
      int fdErr = sopen(path, O_RDONLY | O_CREAT, 0666);
      while((readChar = sread(fdErr,bufErr,MAX_BUF*sizeof(char))) != 0){
        swrite(newsockfd, bufErr, readChar*sizeof(char));
      }
      sclose(fdErr);
    }


    // EXECUTE A PROGRAM
  } else{
    int programId;
    sread(newsockfd,&programId,sizeof(int));
    swrite(newsockfd,&programId,sizeof(int));
    if(programId >= getSize()){
      int req = -2;
      swrite(newsockfd,&req,sizeof(int));
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
        int fdExec = sopen(pathRes, O_CREAT | O_WRONLY| O_TRUNC, 0666);
        int stdout = dup(1);
        checkNeg(stdout, "ERROR dup");
        ret = dup2(fdExec, 1);
        checkNeg(ret, "ERROR dup2");
        long t1 = now();
        fork_and_run1(&exec2,&p.sourceFile);
        wait(&status);
        ret = dup2(stdout, 1);
        checkNeg(ret, "ERROR dup");
        sclose(stdout);
        sclose(fdExec);
        long t2 = now();
        long execTime = t2 - t1;
        p.executionCounter += 1;
        p.executionTime += execTime;
        int progStatus = WEXITSTATUS(status);
        if(WIFEXITED(status) != 0){
          int req = 1;
          swrite(newsockfd,&req,sizeof(int));
          swrite(newsockfd,&execTime,sizeof(long));
          swrite(newsockfd,&progStatus,sizeof(int));
          int fd = sopen(pathRes, O_RDONLY | O_CREAT, 0666);
          char buffer[MAX_SIZE];
          int readChar;
          while  ((readChar = sread(fd, buffer, MAX_SIZE*sizeof(char))) != 0 ){
            swrite(newsockfd, &buffer, readChar*sizeof(char));
          }
          sclose(fd);
        }
        else{
          int req = 0;
          swrite(newsockfd,&req,sizeof(int));
        }
      }
      else{
        p.compilationError = true;
        int req = -1;
        swrite(newsockfd,&req,sizeof(int));
      }
    }
  }
  sclose(newsockfd);
}



int main(int argc, char const *argv[]) {
  int sockfd, newsockfd;
  getSem();
  initShm();
  sockfd = initSocketServer(SERVER_PORT);
  printf("Le serveur tourne sur le port : %i \n",SERVER_PORT);
  while (1) {
    newsockfd = accept(sockfd, NULL,NULL);
    if (newsockfd > 0 ) {
      fork_and_run1(&addProg, &newsockfd);
      sclose(newsockfd);
    }
  }
  sshmdt();
  return 0;
}
