
#ifndef HARP_HARP_H_INCLUDED
#define HARP_HARP_H_INCLUDED

#include <net/ethernet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <net/if.h>
#include "arp.h"

typedef struct harp_desc {
  char               dev[IF_NAMESIZE];
  in_addr_t          vip;
  struct ether_addr  *vh_macs;
/*  uint32_t           *vh_weights; */
  uint32_t           vh_count;
  uint32_t           cur_vh;
  int                sock;
  pthread_rwlock_t   lock;
} harp_desc_t;

struct ether_addr *harp_get_vh_mac(const harp_desc_t *harp, uint32_t i); 
int harp_init(harp_desc_t *harp, const char *dev, const char *vip, uint32_t vh_count, const char **mac_addrs);
void harp_destory(harp_desc_t *harp);

int harp_on_arp_request(harp_desc_t *harp, const arp_op_t *op);
void harp_set_cur_vh(harp_desc_t *harp, uint32_t n);
#endif

