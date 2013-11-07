/**
 *
 *
 */
#include <pcap.h>
#include <stdlib.h>
#include <string.h>


#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAXBYTES2CAPTURE 2048

#include "pcap.h"
#include "harp.h"

int main(int argc, char *argv[]){
  if (argc != 3){
    printf("USAGE: harpd dev vip\n");
    exit(1);
  }
  int ret;
  char errbuf[PCAP_ERRBUF_SIZE];
  memset(errbuf, 0, PCAP_ERRBUF_SIZE);

  struct in_addr addr;
  if (!inet_aton(argv[2], &addr)) {
    fprintf(stderr, "invalid vip addr:\n");
    exit(2);
  }
  harp_desc_t harp;
  harp.vip = addr.s_addr;

  ret = listen_pcap(argv[1], &harp, errbuf, PCAP_ERRBUF_SIZE);
  if (ret != 0) {
    fprintf(stderr, "ERR: %s\n", errbuf);
  }
  return ret;
}


/* EOF */
