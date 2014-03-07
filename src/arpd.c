#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "arp.h"



int main(int argn, char **argc) {
  int sock = arp_socket_init();
  if (argn < 2) {
    return 1;
  }
  char *dev = argc[1];
  arp_listener_t listener = arp_create_listener(sock, dev);
  arp_thread_t thread = arp_create_thread(&listener, &print_arp_op);
  arp_run_thread(&thread);
  char buf[100];
  arp_op_t arp_op;
  while (!feof(stdin)) {
    if (fgets(buf, 100, stdin)) {
      int result = parse_arp_op_str(&arp_op, buf);
      if (result != 0) {
        printf("parse err: %d\n", result);
        continue;
      }
      send_arp(sock, dev, &arp_op);
    }
  }
  arp_stop_thread(&thread);
  return 0;
}

