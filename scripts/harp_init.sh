#!/bin/bash

ip="$1"

if [ -z "$ip" ]; then
   echo "missing arg ip" >&2
   exit 1
fi

ifid=`/sbin/ifconfig|gawk -F'[: ]' 'BEGIN{i=0} $0 ~ "^lo:" { if ($2>i){i=$2} } END{print i+1}'`

echo 1 > /proc/sys/net/ipv4/conf/all/arp_ignore
echo 2 > /proc/sys/net/ipv4/conf/all/arp_announce
/sbin/ifconfig "lo:$ifid" "$1" netmask 255.255.255.255


