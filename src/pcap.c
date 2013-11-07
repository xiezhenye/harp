#include <pcap.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define MAXBYTES2CAPTURE 2048


#include "pcap.h"
#include "harp.h"

void on_packet(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char * packet);

int listen_pcap(const char *dev, const harp_desc_t *p_harp, char *errbuf, const size_t bufsize) {
  bpf_u_int32 netaddr = 0, mask = 0;
  struct bpf_program filter;
  pcap_t *descr = NULL;
  descr = pcap_open_live(dev, MAXBYTES2CAPTURE, 0, 512, errbuf);
  if (descr == NULL){
    return 1;
  }

  if (pcap_lookupnet(dev, &netaddr, &mask, errbuf) == -1){
    return 2;
  }

  if (pcap_compile(descr, &filter, "arp", 1, mask) == -1){
    strncpy(errbuf, pcap_geterr(descr), bufsize);
    return 3;
  }

  if (pcap_setfilter(descr,&filter) == -1){
    strncpy(errbuf, pcap_geterr(descr), bufsize);
    return 4;
  }
  /* main loop */
  if (pcap_loop(descr, -1, on_packet, (u_char *)p_harp) == -1){
    strncpy(errbuf, pcap_geterr(descr), bufsize);
    return 5;
  }
  return 0;
}

void on_packet(u_char *arg, const struct pcap_pkthdr* pkthdr, const u_char * packet) {
  arp_packet_t *p_arp_packet = (arp_packet_t *)packet;
  /*
  printf("Received Packet Size: %d bytes\n", pkthdr->len);
  */
  if (ntohs(p_arp_packet->hw_type) != ETHER_HW_TYPE || ntohs(p_arp_packet->prot_type) != IP_PROTO_TYPE){
    return;
  }
  if (ntohs(p_arp_packet->op) != ARP_REQUEST) {
    return;
  }

  harp_desc_t *p_harp = (harp_desc_t *) arg;
  arp_op_t op = arp_packet_to_arp_op(*p_arp_packet);
  if (p_harp->vip != *(in_addr_t *)(p_arp_packet->rcpt_ip_addr)) {
    return;
  }
  print_arp_op(op);
}

