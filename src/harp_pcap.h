#ifndef HARP_PCAP_H_INCLUDED
#define HARP_PCAP_H_INCLUDED

#include <pcap.h>
#include "arp.h"
#include "harp.h"

int listen_pcap(const char *dev, const harp_desc_t *p_harp, char *errbuf, const size_t bufsize);

#endif
