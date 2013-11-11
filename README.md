HARP
====

An HA utility to drift VIP amount active and standby hosts.

The triditional way to drift VIP is to release the VIP on the old host and then bind it to the new host. It will meet with IP conflict when the first step failed and the second step succeed. Many machinism and tools like watchdog in heartbeat are used to avoid this problem and introduced a lot of complexitis.

HARP uses a different way to drift VIP. VIP is not binded on the ethernet interfaces on any host but just binded on the lo device and set the kernel not to reply arp request not belong the origin interface. Thus, neither the active and the standby hosts can reply arp request directly. The machine address of the active host will be replied by HARP instead and  there is no chance that IP conflict happens. Futhermore, in this way, it is easier to extends to more than two nodes in a cluster. Unlike using proxy to handle HA, no net flow is through HARP so it will not become bottlehold.

Usage 
-----

First, bind VIP and initialize system config on backends using 'script/harp_init.sh'. like

    # ./harp_init.sh 192.168.1.188

Then use 'harp' on another host to answer arp requests and switch amount backends.

  harp dev vip mac [mac ...]

e.g.

    # harp eth0 192.168.1.188 08:00:27:D0:BD:A5 08:00:27:B0:2A:FE
  
The first mac address is mac 0, the second is mac 1 and so on. The default backend is mac 0. Input mac number in stdin per line to switch amount backends. If you need a tcp interface, you can use 

    # nc -k -l <port> | harp ...
    
to create one.    


Limitition
---------- 

harp CAN NOT runs on the frontend client machines. Because the arp packets sent by harp can not be received in the same interface of the same host.
