#include <stdio.h>
#include <string.h>
#include "harp.h"
#include "harp_pcap.h"

int main(int argc, char *argv[]){
  if (argc < 4){
    printf("USAGE: harpd dev vip mac [mac ...]\n");
    return 1; 
  }
  int ret;
  harp_desc_t harp;
  if (!harp_init(&harp, argv[1], argv[2], argc - 3, (const char**)&argv[3])){
    fprintf(stderr, "invalid arg\n");
    return 2;
  }
  char errbuf[PCAP_ERRBUF_SIZE];
  memset(errbuf, 0, PCAP_ERRBUF_SIZE);
  ret = listen_pcap(argv[1], &harp, errbuf, PCAP_ERRBUF_SIZE);
  if (ret != 0) {
    fprintf(stderr, "ERR: %s\n", errbuf);
  }
  return ret;
}


