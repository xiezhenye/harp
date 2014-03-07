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
  struct ether_addr sndr_hw_addr;
  struct in_addr    sndr_ip_addr;
  struct ether_addr rcpt_hw_addr; 
  struct in_addr    rcpt_ip_addr;
  u_short op;
} arp_op_t;

typedef struct arp_listener {
  int sock;
  int running;
  struct timeval tv;
} arp_listener_t;

typedef void(*arp_callback_t)(const arp_op_t *arp);

typedef struct arp_thread {
  arp_listener_t *listener;
  arp_callback_t callback;
  pthread_t    tid;
} arp_thread_t;

arp_op_t arp_packet_to_arp_op(arp_packet_t in);

void build_arp_packet(arp_packet_t *ret, const arp_op_t *in);

int build_arp_op(arp_op_t *ret, u_short op, 
  const char *sndr_hw_addr, const char *sndr_ip_addr, 
  const char *rcpt_hw_addr, const char *rcpt_ip_addr);

int parse_arp_op_str(arp_op_t *ret, const char *line);

void print_mac(const struct ether_addr *mac);

void print_ip(const struct in_addr *ip);

void print_arp_op(const arp_op_t *in);

void print_arp_packet(const arp_packet_t *in);

int arp_socket_init();

arp_listener_t arp_create_listener(int sock);

int arp_start_listener(arp_listener_t *listener, arp_callback_t callback);

void arp_stop_listener(arp_listener_t *listener);

int send_arp(int sock, char *dev, arp_op_t *arp_op);

void build_reply_arp_op(arp_op_t *ret, const arp_op_t *op, const struct ether_addr *result);

void build_gratuitous_arp_op(arp_op_t *ret, const struct ether_addr *mac, const struct in_addr *ip); 

int ip_cmp(const struct in_addr *a, const struct in_addr *b);

int mac_cmp(const struct ether_addr *a, const struct ether_addr *b);

arp_thread_t arp_create_thread(arp_listener_t *listener, arp_callback_t callback);

void arp_run_thread(arp_thread_t *thread);

void arp_stop_thread(arp_thread_t *thread);

#endif

