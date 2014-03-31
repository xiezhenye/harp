package main

import (
  "harp"
)


func main() {
  
  server, err := harp.FromConfig(&harp.HarpConfig{
    Clusters: []harp.ClusterConfig{
      {
        IP: "192.168.56.200",
        Dev: "eth1",
        Nodes: []harp.NodeConfig{
          {
            HwAddr: "f0:f1:f2:f3:f4:f5", 
          },
        },
      },
    },
  })
  if err != nil {
    return
  }
  server.Start()
}



