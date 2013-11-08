#include <stdio.h>
#include <pthread.h>

#include "harp.h"
#include "cli.h"

void start_cli(harp_desc_t *harp) {
  uint32_t n;
  while (scanf("%ud", &n) != EOF) {
    if (n >= harp->vh_count) {
      continue;
    }
    if (pthread_rwlock_wrlock(&(harp->lock))) {
      continue;
    }
    harp->cur_vh = n;
    pthread_rwlock_unlock(&(harp->lock));
  }
}


