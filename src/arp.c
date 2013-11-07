
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
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
  ret->op = op;
  if (ether_aton_r(sndr_hw_addr, &(ret->sndr_hw_addr)) == NULL) {
    return 1;
  }
  if (!inet_aton(sndr_ip_addr, &(ret->sndr_ip_addr))) {
    return 2;
  }
  if (ether_aton_r(rcpt_hw_addr, &(ret->rcpt_hw_addr)) == NULL) {
    return 3;
  }
  if (!inet_aton(rcpt_ip_addr, &(ret->rcpt_ip_addr))) {
    return 4;
  }
  return 0;
}

arp_packet_t build_arp_packet(arp_op_t in) {
  arp_packet_t ret;
  ret.frame_type = ARP_FRAME_TYPE;
  ret.hw_type = ETHER_HW_TYPE;
  ret.prot_type = IP_PROTO_TYPE;
  ret.hw_addr_size = ETH_HW_ADDR_LEN;
  ret.prot_addr_size = IP_ADDR_LEN;
  memcpy(&(ret.targ_hw_addr), &(in.rcpt_hw_addr), ETH_HW_ADDR_LEN);
  memcpy(&(ret.src_hw_addr), &(in.sndr_hw_addr), ETH_HW_ADDR_LEN); 
  ret.op = in.op;
  memcpy(ret.sndr_hw_addr, &(in.sndr_hw_addr), ETH_HW_ADDR_LEN);
  memcpy(ret.sndr_ip_addr, &(in.sndr_ip_addr), IP_ADDR_LEN); 
  memcpy(ret.rcpt_hw_addr, &(in.rcpt_hw_addr), ETH_HW_ADDR_LEN); 
  memcpy(ret.rcpt_ip_addr, &(in.rcpt_ip_addr), IP_ADDR_LEN); 
  bzero(ret.padding, 18);
  return ret; 
}

void print_mac(struct ether_addr mac) {
  char *t = (char *)(&mac);
  printf("%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX",
    t[0],t[1],t[2],t[3],t[4],t[5]);
}

void print_ip(struct in_addr ip) {
  unsigned char *t = (unsigned char *)(&ip);
  printf("%d.%d.%d.%d", 
    t[0],t[1],t[2],t[3]);
}

void print_arp_op(arp_op_t in) {
  print_mac(in.sndr_hw_addr);
  printf(" ");
  print_ip(in.sndr_ip_addr);
  printf(ntohs(in.op) == ARP_REQUEST ? " rqst " : " rply ");
  print_mac(in.rcpt_hw_addr);
  printf(" ");
  print_ip(in.rcpt_ip_addr);
  printf("\n");
}

void print_arp_packet(arp_packet_t in) {
  
}
/*
int main(){
  arp_op_t op;
  if (build_arp_op(&op, ARP_REQUEST, 
      "01:02:03:04:05:06",
      "1.2.3.4",
      "a1:b2:c3:d4:e5:f6",
      "101.102.103.104") != 0) {
    return 1;
  }
  print_arp_op(op);
  return 0;
}
*/
