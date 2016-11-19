
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "delcom_linux.h"

#define BUFLEN 512
#define PORT 9930

static HIDPacketStruct my_packet;

int write_hid(hid_device *hnd, uint8_t data) {
  my_packet.tx.major_cmd = WRITE_PORT_CMD;
  my_packet.tx.minor_cmd = 2;
  my_packet.tx.data_lsb = data;
  return hid_send_feature_report(hnd, my_packet.data, 8);
}

int main() {
  struct sockaddr_in si_me, si_other;
  int s;
  socklen_t slen = sizeof(si_other);
  int status;
  char buf[BUFLEN];
  char *substr;
  hid_device *handle;

  handle = hid_open(DELCOM_HID_VID, DELCOM_HID_PID, NULL);
  if (!handle) {
    printf("Error: Unable to open hid device. (%d:%s)\n", errno, strerror(errno));
    exit(errno);
  }

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

  while (true) {
    status = recvfrom(s, buf, BUFLEN, 0, &si_other, &slen);
    if (status < 0) {
      printf("Failed to receive data (%d:%s)\n", errno, strerror(errno));
      exit(errno);
    }

    substr = strstr(buf, "STARTED");
    if (substr != NULL) {
      printf("Found STARTED. Light yellow LED\n");
      write_hid(handle, YELLOW_COLOR);
      continue;
    }

    substr = strstr(buf, "SUCCESS");
    if (substr != NULL) {
      printf("Found SUCCESS. Light green LED\n");
      write_hid(handle, GREEN_COLOR);
      continue;
    }

    substr = strstr(buf, "FAILURE");
    if (substr != NULL) {
      printf("Found FAILURE. Light red LED\n");
      write_hid(handle, RED_COLOR);
      continue;
    }
  }

  hid_close(handle);
  hid_exit();
  close(s);
  return 0;
}
