#!/bin/bash

ip="$1"

if [ -z "$ip" ]; then
   echo "missing arg ip" >&2
   exit 1
fi

echo 1 >/proc/sys/net/ipv4/conf/all/arp_ignore
echo 2 >/proc/sys/net/ipv4/conf/all/arp_announce

ip addr add "$1/32" dev lo
if [ "$?" -ne 0 ]; then
  echo "bind ip $1 to lo failed" >&2
  exit 3
fi

