package harp

import (
  "net"
  "arp"
)

type HarpServer struct {
  Clusters []HaCluster
  listener arp.ArpListener
}

type HaNode struct {
  HwAddr  net.HardwareAddr
}

type HaCluster struct {
  IP   net.IP
  Dev  *net.Interface
  Nodes []HaNode
  Current int
}

type HarpConfig struct {
  Clusters []ClusterConfig
}

type ClusterConfig struct {
  IP   string
  Dev  string
  Nodes []NodeConfig 
}

type NodeConfig struct {
  HwAddr string
}

func (self *HaCluster) CurrentNode() HaNode {
  return self.Nodes[self.Current]
}

func NewHarpServer() HarpServer {
  var ret HarpServer
  ret.Clusters = make([]HaCluster, 0, 4)
  return ret
}

func FromConfig(conf *HarpConfig) (ret HarpServer, err error) {
  ret = NewHarpServer()
  for _, clusterConf := range conf.Clusters {
    cluster := NewCluster()
    cluster.IP = net.ParseIP(clusterConf.IP)
    cluster.Dev, err = net.InterfaceByName(clusterConf.Dev)
    if err != nil {
      return
    }
    for _, nodeConf := range clusterConf.Nodes {
      var node HaNode
      node.HwAddr, err = net.ParseMAC(nodeConf.HwAddr)
      if err != nil {
        return
      }
      cluster.AddNode(node)
    }
    ret.AddCluster(cluster)
  }
  return
}

func NewCluster() HaCluster {
  var ret HaCluster
  ret.Nodes = make([]HaNode, 0, 2)
  return ret
}

func (self *HarpServer) AddCluster(cluster HaCluster) {
  self.Clusters = append(self.Clusters, cluster)
}

func (self *HarpServer) SwitchTo(clusterIndex, nodeIndex int) {
  cluster := &self.Clusters[clusterIndex]
  cluster.SwitchTo(nodeIndex)
  self.listener.Send(arp.GratuitousArpOp(cluster.IP, cluster.CurrentNode().HwAddr), arp.AddrFromInterface(cluster.Dev))
}

func (self *HaCluster) AddNode(node HaNode) {
  self.Nodes = append(self.Nodes, node)
}

func (self *HaCluster) SwitchTo(n int) {
  if n >= len(self.Nodes) {
    return
  }
  self.Current = n
}

func (self *HarpServer) Start() (err error) {
  self.listener, err = arp.NewListener() 
  if err != nil {
    return
  }
  defer self.listener.Close()
  self.listener.Listen(func(op arp.ArpOp, addr arp.Addr) {
    if op.Op != arp.ARP_REQUEST {
      return
    }
    for _, cluster := range self.Clusters {
      if cluster.IP.Equal(op.RcptIpAddr) {
        var reply arp.ArpOp
        reply.Op = arp.ARP_REPLY
        reply.SndrIpAddr = op.RcptIpAddr
        reply.SndrHwAddr = cluster.CurrentNode().HwAddr
        reply.RcptIpAddr = op.SndrIpAddr
        reply.RcptHwAddr = op.SndrHwAddr
        self.listener.Send(reply, addr)
      }
    } 
  })
  return
}


