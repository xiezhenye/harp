#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "harp.h"
#include "harp_pcap.h"
#include "cli.h"
#include "ping.h"

int main(int argc, char *argv[]){
  if (argc < 4){
    fprintf(stderr, "USAGE: harpd dev vip mac [mac ...]\n");
    return 1; 
  }
  harp_desc_t harp;
  if (!harp_init(&harp, argv[1], argv[2], argc - 3, (const char**)&argv[3])){
    fprintf(stderr, "invalid arg\n");
    return 2;
  }
  int r = start_pcap_thread(&harp);
  if (!r) {
    fprintf(stderr, "pcap thread failed\n");
    return 3;
  }
  start_cli(&harp);
  stop_pcap_thread(&harp);
  harp_destory(&harp);
  return 0;
}

