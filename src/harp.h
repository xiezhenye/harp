
#ifndef HARP_HARP_H_INCLUDED
#define HARP_HARP_H_INCLUDED

#include <net/ethernet.h>
#include <netinet/in.h>
#include <arpa/inet.h>


typedef struct harp_desc {
  in_addr_t          vip;
  struct ether_addr  *vh_macs;
/*  uint32_t           *vh_weights; */
  uint32_t           vh_count;
} harp_desc_t;

struct ether_addr *harp_select_vh(const harp_desc_t *harp, uint32_t i); 
int harp_init(harp_desc_t *harp, const char *dev, const char *vip, uint32_t vh_count, const char **mac_addrs);
void harp_free(harp_desc_t *harp);
#endif

