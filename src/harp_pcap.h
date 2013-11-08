#ifndef HARP_PCAP_H_INCLUDED
#define HARP_PCAP_H_INCLUDED

#include "arp.h"
#include "harp.h"

int start_pcap_thread(harp_desc_t *harp);
int stop_pcap_thread(harp_desc_t *harp);
#endif

