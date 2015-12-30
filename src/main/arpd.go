package main

import (
  "arp"
  "fmt"
  "flag"
  "config"
  "net"
)

func parseFlags(conf *string, dev *string) {
  flag.StringVar(conf, "conf", "", "config description.\n"+ 
                 "e.g. file:/path/to/file;\n"+
                 "      etcd:node_name@10.10.10.1:2379,10.10.10.2:2379/prefix")
  flag.StringVar(dev, "dev", "", "network device. e.g. eth0")
  flag.Parse()
}

func main() {
  var confDesc, devName string
  parseFlags(&confDesc, &devName)
  dev, err := net.InterfaceByName(devName)
  listener, err := arp.NewListener()
  if err != nil {
    fmt.Println("listenning raw socket failed")
    fmt.Println(err)
    return
  }
  defer listener.Close()

  conf, err := config.NewConfig(confDesc)
  if err != nil {
    fmt.Println("load config file failed")
    fmt.Println(err)
    return
  }
 
  conf.SetOnChange(func(ip string, mac string){
    if mac == "" {
      return
    }
    ipAddr:= net.ParseIP(ip)
    hwAddr, err:= net.ParseMAC(mac)
    if err != nil {
      fmt.Println("parse mac addr failed: " + mac)
      return
    }
    fmt.Printf("%s => %s\n", ipAddr, hwAddr)
    g := arp.GratuitousArpOp(ipAddr, hwAddr)
    listener.Send(g, arp.AddrFromInterface(dev))
  })
  listener.Listen(func(op arp.ArpOp, addr arp.Addr){
    if op.Op != arp.ARP_REQUEST {
      return
    }
    mac := conf.GetMacByIp(op.RcptIpAddr.String())
    //fmt.Printf("%s -> %s\n", op.RcptIpAddr.String(), mac)
    if mac == "" {
      return
    }
    var reply arp.ArpOp
    reply.Op = arp.ARP_REPLY
    reply.SndrIpAddr = op.RcptIpAddr
    reply.SndrHwAddr, err = net.ParseMAC(mac)
    if err != nil {
      fmt.Println("parse mac addr failed: " + mac)
      return
    }
    reply.RcptIpAddr = op.SndrIpAddr
    reply.RcptHwAddr = op.SndrHwAddr
    listener.Send(reply, addr)
  })
}



