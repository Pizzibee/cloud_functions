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


int main(int argc, char const *argv[]) {
  printf("Server hello\n");
  int sockfd, newsockfd, ret;

  initShm();
  sockfd = initSocketServer(SERVER_PORT);
  printf("Le serveur tourne sur le port : %i \n",SERVER_PORT);
  while (1) {
    /* client trt */
    newsockfd = accept(sockfd, NULL,NULL);
    if (newsockfd > 0 ) {
      int req;
			Program p;
      ret = read(newsockfd,&req,sizeof(int));
      checkNeg(ret,"server read choice error");
      if (req == -1) {
        int sizeName;
				char fileName[255];
				char path[255] = "serverPrograms/";
				char buf[150];
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
				while((readChar = read(newsockfd,buf,150*sizeof(char))) != 0){
					ret = write(fd, buf, readChar*sizeof(char));
					checkNeg(ret,"server write file error");
				}
				close(fd);
				strcpy(p.sourceFile,fileName);
				p.executionCounter = 1;
				int id = addProgram(p);
				ret = write(newsockfd,&id,sizeof(int));
				checkNeg(ret,"server write id size error");
				ret = write(newsockfd,&buf,150*sizeof(char));
      } else{
				int programId;
				ret = read(newsockfd,&programId,sizeof(int));
				checkNeg(ret,"server read choice error");
				printf("%d\n", programId);
				p = getProgram(programId);
      }
    }
    close(newsockfd);
  }
  sshmdt();
  return 0;
}
