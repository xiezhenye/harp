package main

import (
  "arp"
  "fmt"
  "config"
)

func parseFlags(conf *string) {
  flag.StringVar(conf, "conf", "", "config description.\n"
                 "e.g. file:/path/to/file;\n"
                 "      etcd:node_name@10.10.10.1:2379,10.10.10.2:2379/prefix")
  flag.Parse()
}

func main() {
  var confDesc string
  parseFlags(&confDesc)
  conf, err := config.NewConfig(confDesc)
  if err != nil {
    return
  }
  
  listener, err := arp.NewListener()
  if err != nil {
    return
  }
  defer listener.Close()
  listener.Listen(func(op arp.ArpOp, addr arp.Addr){
    if op.Op != arp.ARP_REQUEST {
      return
    }
    mac := conf.GetMacByIp(op.RcptIpAddr)
    if mac == "" {
      return
    }
    var reply arp.ArpOp
    reply.Op = arp.ARP_REPLY
    reply.SndrIpAddr = op.RcptIpAddr
    reply.SndrHwAddr = cluster.CurrentNode().HwAddr
    reply.RcptIpAddr = op.SndrIpAddr
    reply.RcptHwAddr = op.SndrHwAddr
    listener.Send(reply, addr)
  })
}



