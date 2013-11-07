#ifndef HARP_ARP_H_INCLUDED
#define HARP_ARP_H_INCLUDED

#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define ARP_REQUEST 1
#define ARP_REPLY 2

#define ETH_HW_ADDR_LEN 6
#define IP_ADDR_LEN 4
#define ARP_FRAME_TYPE 0x0806
#define ETHER_HW_TYPE 1
#define IP_PROTO_TYPE 0x0800

typedef struct arp_packet {
  u_char targ_hw_addr[ETH_HW_ADDR_LEN];
  u_char src_hw_addr[ETH_HW_ADDR_LEN];
  u_short frame_type;
  u_short hw_type;
  u_short prot_type;
  u_char hw_addr_size;
  u_char prot_addr_size;
  u_short op;
  u_char sndr_hw_addr[ETH_HW_ADDR_LEN];
  u_char sndr_ip_addr[IP_ADDR_LEN];
  u_char rcpt_hw_addr[ETH_HW_ADDR_LEN];
  u_char rcpt_ip_addr[IP_ADDR_LEN];
  u_char padding[18];
} arp_packet_t;

typedef struct arp_op {
  u_short op;
  struct ether_addr sndr_hw_addr;
  struct in_addr    sndr_ip_addr;
  struct ether_addr rcpt_hw_addr; 
  struct in_addr    rcpt_ip_addr;
} arp_op_t;

arp_op_t arp_packet_to_arp_op(arp_packet_t in);
arp_op_t *arp_packet_p_to_arp_op_p(arp_packet_t *in);
arp_packet_t build_arp_packet(arp_op_t in);
int build_arp_op(arp_op_t *ret, u_short op, const char *sndr_hw_addr, const char *sndr_ip_addr, const char *rcpt_hw_addr, const char *rcpt_ip_addr);
void print_arp_op(arp_op_t in);
void send_arp(arp_op_t arp_op);

#endif

