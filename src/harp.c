/**
 *
 *
 */
#include <pcap.h>
#include <stdlib.h>
#include <string.h>


#include <net/ethernet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXBYTES2CAPTURE 2048

#include "harp_pcap.h"
#include "harp.h"

struct ether_addr *harp_select_vh(const harp_desc_t *harp, uint32_t i) {
  if (i >= harp->vh_count) {
    return NULL;
  }
  return &(harp->vh_macs[i]);
}

int harp_init(harp_desc_t *harp, const char *dev, const char *vip, uint32_t vh_count, const char **mac_addrs) {
  struct in_addr addr;
  if (!inet_aton(vip, &addr)) {
    return 0;
  }
  harp->vip = addr.s_addr;
  harp->vh_count = vh_count;
  harp->vh_macs = calloc(harp->vh_count, sizeof(struct ether_addr));
  /* mac addrs */
  int i;
  for (i = 0; i < vh_count; i++) {
    if (!ether_aton_r(mac_addrs[i], &(harp->vh_macs[i]))) {
      return 0;
    }
  }
  return 1;
}

void harp_free(harp_desc_t *harp) {
  free(harp->vh_macs);
}

int harp_on_arp_request(harp_desc_t *harp, arp_op_t op) {
  return 0;
}

/* EOF */
