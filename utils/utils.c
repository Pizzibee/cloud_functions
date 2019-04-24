#include "utils.h"

/* Convert a DNS domain name to IP v4
 Necessary for socket connection because IP required */
int hostname_to_ip(char *hostname, char *ip) {
  struct hostent *he;
  struct in_addr **addr_list;
  int i;

  if ((he = gethostbyname(hostname)) == NULL) {
    herror("gethostbyname");
    return 1;
  }

  addr_list = (struct in_addr **)he->h_addr_list;

  for (i = 0; addr_list[i] != NULL; i++) {
    strcpy(ip, inet_ntoa(*addr_list[i]));
    return 0;
  }
  return 1;
}

/*
        buffered read
*/
int readline(int fd, char *line) {
  static int nbChar;
  static char buffer[BUFFER_SIZE];
  static int i = 0;
  int j = 0;

  while (1) {
    if ((i == 0) || (i == BUFFER_SIZE)) {
      nbChar = read(fd, buffer, BUFFER_SIZE);
      i = 0;
      if (nbChar == 0)
        return 0;
    }

    if (i >= (nbChar - 1))
      return 0;

    while (i < BUFFER_SIZE) {
      if (buffer[i] == '\n') {
        /* return line is a string */
        line[j] = '\0';
        i++; // pass \n for next
        // printf("i : %i\n",i);

        return 1;
      } else {

        line[j] = buffer[i];
        i++;
        j++;
      }
    }
  }
}
