#include <stdio.h>

#include "harp.h"
#include "cli.h"

void start_cli(harp_desc_t *harp) {
  uint32_t n;
  while (scanf("%ud", &n) != EOF) {
    if (n >= harp->vh_count) {
      continue;
    }
    harp_set_cur_vh(harp, n);
  }
}


