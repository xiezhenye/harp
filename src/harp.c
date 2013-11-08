/**
 *
 *
 */
#include <pcap.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#define MAXBYTES2CAPTURE 2048

#include "harp_pcap.h"
#include "harp.h"
#include "arp.h"

struct ether_addr *harp_get_vh_mac(const harp_desc_t *harp, uint32_t i) {
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
  strncpy(harp->dev, dev, IF_NAMESIZE - 1);
  harp->vip = addr.s_addr;
  harp->vh_count = vh_count;
  harp->vh_macs = calloc(harp->vh_count, sizeof(struct ether_addr));
  harp->cur_vh = 0;
  /* mac addrs */
  int i;
  for (i = 0; i < vh_count; i++) {
    if (!ether_aton_r(mac_addrs[i], &(harp->vh_macs[i]))) {
      return 0;
    }
  }
  pthread_rwlock_init(&(harp->lock), NULL);
  harp->sock = arp_socket_init(); 
  return 1;
}

void harp_destory(harp_desc_t *harp) {
  free(harp->vh_macs);
  pthread_rwlock_destroy(&(harp->lock));
  close(harp->sock);
}

void harp_set_cur_vh(harp_desc_t *harp, uint32_t n) {
  if (pthread_rwlock_wrlock(&(harp->lock))) {
    return;
  }
  harp->cur_vh = n;
  pthread_rwlock_unlock(&(harp->lock));
}

struct ether_addr *harp_get_cur_vh_mac(harp_desc_t *harp) {
  struct ether_addr *ret;
  if (pthread_rwlock_rdlock(&(harp->lock))) {
    return NULL;
  }
  ret = harp_get_vh_mac(harp, harp->cur_vh);  
  if (pthread_rwlock_unlock(& harp->lock)) {
    return NULL;
  }
  return ret;
}

int harp_on_arp_request(harp_desc_t *harp, const arp_op_t *op) {
  struct ether_addr mac;
  arp_op_t rpl;
  mac = *harp_get_cur_vh_mac(harp);
  build_reply_arp_op(&rpl, op, &mac); 
  send_arp(harp->sock, harp->dev, &rpl);
  return 0;
}

/* EOF */
