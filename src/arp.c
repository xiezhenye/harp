#include <sys/types.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <string.h>
#include <stdio.h>
#include <net/if.h>
#include "arp.h"
#include <errno.h>
#include <pthread.h>

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

void print_mac(const struct ether_addr *mac) {
  char *t = (char *)(mac);
  printf("%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX",
    t[0],t[1],t[2],t[3],t[4],t[5]);
}

void print_ip(const struct in_addr *ip) {
  unsigned char *t = (unsigned char *)(ip);
  printf("%d.%d.%d.%d", 
    t[0],t[1],t[2],t[3]);
}

void print_arp_op(const arp_op_t *in) {
  print_mac(&in->sndr_hw_addr);
  printf(" ");
  print_ip(&in->sndr_ip_addr);
  printf(ntohs(in->op) == ARP_REQUEST ? " rqst " : " rply ");
  print_mac(&in->rcpt_hw_addr);
  printf(" ");
  print_ip(&in->rcpt_ip_addr);
  printf("\n");
}

void print_arp_packet(const arp_packet_t *in) {
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

void build_gratuitous_arp_op(arp_op_t *ret, const struct ether_addr *mac, const struct in_addr *ip) {
  ret->op = htons(ARP_REQUEST);
  ret->sndr_hw_addr = *mac;
  ret->sndr_ip_addr = *ip;
  memset(&ret->rcpt_hw_addr, '\xff', ETH_HW_ADDR_LEN);
  ret->rcpt_ip_addr = *ip;
}

int ip_cmp(const struct in_addr *a, const struct in_addr *b) {
  return memcmp(a, b, sizeof(struct in_addr));
}

int mac_cmp(const struct ether_addr *a, const struct ether_addr *b) {
  return memcmp(a, b, sizeof(struct ether_addr));
}

arp_listener_t arp_create_listener(int sock) {
  arp_listener_t ret;
  ret.sock = sock;
  ret.running = 1;
  ret.tv.tv_sec = 1;
  ret.tv.tv_usec = 0;
  return ret;
}

int arp_start_listener(arp_listener_t *listener, arp_callback_t callback) {
  int sock = listener->sock;
  char buf[ETH_FRAME_LEN];
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
    (char *)&(listener->tv), sizeof(struct timeval));
  
  while (listener->running) {
    ssize_t size = recv(sock, buf, ETH_FRAME_LEN, 0);
    arp_packet_t *packet;
    if (size <= 0) {
      if (errno == EAGAIN) {
        continue;
      } else {
        printf("ERR: %d %s\n", errno, strerror(errno));
        return errno;
      }
    }
    packet = (arp_packet_t *)buf;
    arp_op_t arp = arp_packet_to_arp_op(*packet);
    callback(&arp);
  }
  return 0;
}

void arp_stop_listener(arp_listener_t *listener) {
  listener->running = 0;
}

int sock_bind_dev(int sock, char *dev) {
  return setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, dev, strlen(dev));
}

void *arp_thread_entry(void *arg) {
  arp_thread_t *t = (arp_thread_t *) arg; 
  arp_start_listener(t->listener, t->callback);
  return NULL;
}

arp_thread_t arp_create_thread(arp_listener_t *listener, arp_callback_t callback) {
  arp_thread_t ret;
  ret.listener = listener;
  ret.callback = callback;
  ret.tid = 0;
  return ret;
}

void arp_run_thread(arp_thread_t *thread) {
  pthread_attr_t attr;
  if (pthread_attr_init(&attr)) {
    return;
  }
  if (pthread_create(&(thread->tid), &attr, &arp_thread_entry, thread)) {
    return;
  }
  pthread_attr_destroy(&attr);
}

void arp_stop_thread(arp_thread_t *thread) {
  long ret;
  if (thread->listener != NULL) {
    arp_stop_listener(thread->listener);
  }
  if (thread->tid != 0) {
    pthread_join(thread->tid, (void **)&ret);
  }
  thread->tid = 0;
}


/*
98:0C:82:EA:6C:0A 192.168.1.164 rqst 00:00:00:00:00:00 192.168.1.134
A8:26:D9:BC:9F:13 192.168.1.102 rply FF:FF:FF:FF:FF:FF 192.168.1.1
*/
int parse_arp_op_str(arp_op_t *ret, const char *line) {
  /* 21 * 2 + 15 * 2 + 4 + 5; */
  #define MAX_LINE_LENGTH 81
  char seg[MAX_LINE_LENGTH];
  char *t;
  strncpy(seg, line, MAX_LINE_LENGTH);
  seg[MAX_LINE_LENGTH - 1] = '\0';
  char *mac_from = strtok_r(seg, " ", &t);
  if (mac_from == NULL) {
    return 1;
  }
  char *ip_from = strtok_r(NULL, " ", &t);
  if (ip_from == NULL) {
    return 2;
  }
  char *s_op = strtok_r(NULL, " ", &t); 
  if (s_op == NULL) {
    return 3;
  }
  u_short op;
  if (strcmp(s_op, "rply") == 0) {
    op = ARP_REPLY;
  } else if (strcmp(s_op, "rqst") == 0) {
    op = ARP_REQUEST;
  } else {
    return 3;
  }
  char *mac_to = strtok_r(NULL, " ", &t);
  if (mac_to == NULL) {
    return 4;
  }
  char *ip_to = strtok_r(NULL, " ", &t);
  if (ip_to == NULL) {
    return 5;
  }
  int result;
  result = build_arp_op(ret, op, mac_from, ip_from, mac_to, ip_to);
  if (result != 0) {
    return result + 10;
  }
  return 0;
}


