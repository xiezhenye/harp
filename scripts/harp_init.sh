#!/bin/bash

ip="$1"

if [ -z "$ip" ]; then
   echo "missing arg ip" >&2
   exit 1
fi

echo 1 >/proc/sys/net/ipv4/conf/all/arp_ignore
echo 2 >/proc/sys/net/ipv4/conf/all/arp_announce

ifid=`/sbin/ifconfig|gawk -F'[: ]+' 'BEGIN{i=0} $0 ~ "^lo:" { if ($2>i){i=$2} } $4=="'$ip'"&& $2=="inet"{exit 1} END{print i+1}'`
if [ "$?" -ne 0 ]; then
  echo "ip $ip already exists" >&2
  exit 2
fi

/sbin/ifconfig "lo:$ifid" "$1" netmask 255.255.255.255
if [ "$?" -ne 0 ]; then
  echo "bind ip $1 to lo:$ifid failed" >&2
  exit 3
fi

