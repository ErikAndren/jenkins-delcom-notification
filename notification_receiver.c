
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

int main() {
  struct sockaddr_in si_me, si_other;
  int s, i;
  socklen_t slen = sizeof(si_other);
  int status;
  char buf[BUFLEN];
  char *substr;

  s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s < 0) {
    printf("Failed to open socket (%d:%s)\n", errno, strerror(errno));
    exit(errno);
  }

  memset((char *) &si_me, 0, sizeof(si_me));
  si_me.sin_family = AF_INET;
  si_me.sin_port = htons(PORT);
  si_me.sin_addr.s_addr = htonl(INADDR_ANY);

  status = bind(s, &si_me, sizeof(si_me));
  if (status < 0) {
    printf("Failed to bind to ip address (%d:%s)\n", errno, strerror(errno));
    exit(errno);
  }

  for (i = 0; i < NPACK; i++) {
    status = recvfrom(s, buf, BUFLEN, 0, &si_other, &slen);
    if (status < 0) {
      printf("Failed to receive data (%d:%s)\n", errno, strerror(errno));
      exit(errno);
    }

    substr = strstr(buf, "STARTED");
    if (substr != NULL) {
      printf("Found STARTED. Light yellow LED\n");
      continue;
    }

    substr = strstr(buf, "SUCCESS");
    if (substr != NULL) {
      printf("Found SUCCESS. Light green LED\n");
      continue;
    }

    substr = strstr(buf, "FAILURE");
    if (substr != NULL) {
      printf("Found FAILURE. Light red LED\n");
      continue;
    }

    //    printf("Received packet from %s:%d\nData: %s\n\n",
    //           inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
  }

  close(s);
  return 0;
}
