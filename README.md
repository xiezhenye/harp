harp
====

Reliable active - standby HA utilities.





Usage 
-----

First, bind VIP and initialize system config on backends using 'script/harp_init.sh'. like

  # ./harp_init.sh 192.168.1.188

Then use 'harp' to answer arp requests and switch amount backends.

  harp dev vip mac [mac ...]

e.g.

  # harp eth0 192.168.1.188 08:00:27:D0:BD:A5 08:00:27:B0:2A:FE
  
The first mac address is mac 0, the second is mac 1 and so on. The default backend is mac 0. Input mac number in stdin per line to switch amount backends.


