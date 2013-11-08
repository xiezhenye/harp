#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "harp.h"
#include "harp_pcap.h"
#include "cli.h"

int main(int argc, char *argv[]){
  if (argc < 4){
    printf("USAGE: harpd dev vip mac [mac ...]\n");
    return 1; 
  }
  int ret = 0;
  harp_desc_t harp;
  if (!harp_init(&harp, argv[1], argv[2], argc - 3, (const char**)&argv[3])){
    fprintf(stderr, "invalid arg\n");
    return 2;
  }

  pthread_t tid;
  tid = start_pcap_thread(&harp);
  if (!tid) {
    fprintf(stderr, "pcap thread failed\n");
    return 3;
  }

  start_cli(&harp);
  /*pthread_join(tid, (void *)&ret);*/

  harp_destory(&harp);
  return ret;
}

