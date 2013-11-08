
 
#include <sys/types.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <string.h>
#include <stdio.h>
#include <net/if.h>

#include "arp.h"

arp_op_t arp_packet_to_arp_op(arp_packet_t in) {
  arp_op_t ret;
  ret.op = in.op;
  memcpy(&(ret.sndr_hw_addr), in.sndr_hw_addr, ETH_HW_ADDR_LEN);
  memcpy(&(ret.sndr_ip_addr), in.sndr_ip_addr, IP_ADDR_LEN); 
  memcpy(&(ret.rcpt_hw_addr), in.rcpt_hw_addr, ETH_HW_ADDR_LEN); 
  memcpy(&(ret.rcpt_ip_addr), in.rcpt_ip_addr, IP_ADDR_LEN); 
  return ret; 
}

int build_arp_op(arp_op_t *ret, u_short op, 
  const char *sndr_hw_addr, const char *sndr_ip_addr, 
  const char *rcpt_hw_addr, const char *rcpt_ip_addr) {
  ret->op = htons(op);
  if (!ether_aton_r(sndr_hw_addr, &(ret->sndr_hw_addr))) {
    return 1;
  }
  if (!inet_aton(sndr_ip_addr, &(ret->sndr_ip_addr))) {
    return 2;
  }
  if (!ether_aton_r(rcpt_hw_addr, &(ret->rcpt_hw_addr))) {
    return 3;
  }
  if (!inet_aton(rcpt_ip_addr, &(ret->rcpt_ip_addr))) {
    return 4;
  }
  return 0;
}

void build_arp_packet(arp_packet_t *ret, const arp_op_t *in) {
  ret->frame_type = htons(ARP_FRAME_TYPE);
  ret->hw_type = htons(ETHER_HW_TYPE);
  ret->prot_type = htons(IP_PROTO_TYPE);
  ret->hw_addr_size = ETH_HW_ADDR_LEN;
  ret->prot_addr_size = IP_ADDR_LEN;
  memcpy(&(ret->targ_hw_addr), &(in->rcpt_hw_addr), ETH_HW_ADDR_LEN);
  memcpy(&(ret->src_hw_addr), &(in->sndr_hw_addr), ETH_HW_ADDR_LEN); 
  ret->op = in->op;
  memcpy(ret->sndr_hw_addr, &(in->sndr_hw_addr), ETH_HW_ADDR_LEN);
  memcpy(ret->sndr_ip_addr, &(in->sndr_ip_addr), IP_ADDR_LEN); 
  memcpy(ret->rcpt_hw_addr, &(in->rcpt_hw_addr), ETH_HW_ADDR_LEN); 
  memcpy(ret->rcpt_ip_addr, &(in->rcpt_ip_addr), IP_ADDR_LEN); 
  bzero(ret->padding, 18);
}

void print_mac(struct ether_addr *mac) {
  char *t = (char *)(mac);
  printf("%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX",
    t[0],t[1],t[2],t[3],t[4],t[5]);
}

void print_ip(struct in_addr *ip) {
  unsigned char *t = (unsigned char *)(ip);
  printf("%d.%d.%d.%d", 
    t[0],t[1],t[2],t[3]);
}

void print_arp_op(arp_op_t *in) {
  print_mac(&in->sndr_hw_addr);
  printf(" ");
  print_ip(&in->sndr_ip_addr);
  printf(ntohs(in->op) == ARP_REQUEST ? " rqst " : " rply ");
  print_mac(&in->rcpt_hw_addr);
  printf(" ");
  print_ip(&in->rcpt_ip_addr);
  printf("\n");
}

void print_arp_packet(arp_packet_t *in) {
  print_mac((struct ether_addr *)in->targ_hw_addr);
  printf("\n");
  print_mac((struct ether_addr *)in->src_hw_addr);
  printf("\n");
  printf("0x%04hX\n0x%04hX\n0x%04hX\n%hhd\n%hhd\n0x%04hX\n",
    ntohs(in->frame_type), ntohs(in->hw_type), ntohs(in->prot_type),
    in->hw_addr_size, in->prot_addr_size, ntohs(in->op));
  print_mac((struct ether_addr *)in->sndr_hw_addr);
  printf("\n");
  print_ip((struct in_addr *)in->sndr_ip_addr);
  printf("\n");
  print_mac((struct ether_addr *)in->rcpt_hw_addr);
  printf("\n");
  print_ip((struct in_addr *)in->rcpt_ip_addr);
  printf("\n\n");
}

int arp_socket_init() {
  int sock = socket(AF_INET,SOCK_PACKET,htons(ETH_P_ARP));
  return sock;
}

int send_arp(int sock, char *dev, arp_op_t *arp_op) {
  struct sockaddr sa;
  arp_packet_t pkt;
  strncpy(sa.sa_data, dev, IF_NAMESIZE - 1);
  build_arp_packet(&pkt, arp_op);
  if (sendto(sock, &pkt, sizeof(pkt), 0, &sa, sizeof(sa)) < 0){
    return 0; 
  }
  return 1;
}

void build_reply_arp_op(arp_op_t *ret, const arp_op_t *op, const struct ether_addr *result) {
  ret->op = htons(ARP_REPLY);  
  ret->sndr_hw_addr = *result;
  ret->sndr_ip_addr = op->rcpt_ip_addr;
  ret->rcpt_hw_addr = op->sndr_hw_addr;
  ret->rcpt_ip_addr = op->sndr_ip_addr;
}

/*
int main(){
  arp_op_t op;
  if (build_arp_op(&op, ARP_REPLY, 
      "01:02:03:04:05:06",
      "192.168.1.222",
      "08:00:27:D0:BD:A5",
      "192.168.1.143") != 0) {
    return 1;
  }
  print_arp_op(&op);
  int sock = arp_socket_init();
  send_arp(sock, "eth0", op);
  close(sock);
  return 0;
}
*/

