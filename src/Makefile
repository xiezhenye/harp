harp: harp.c harp_pcap.c arp.c cli.c main.c
	gcc -Wall harp.c harp_pcap.c arp.c cli.c main.c -g -lpcap -lpthread -o harp

arpd: arp.c arpd.c
	gcc -Wall arp.c arpd.c -lpthread -g -o arpd

clean:
	rm -f harp
