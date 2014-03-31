package main

import (
  "arp"
  "fmt"
)


func main() {
  listener, err := arp.NewListener()
  if err != nil {
    return
  }
  listener.Listen(func(op arp.ArpOp, addr arp.Addr){
    fmt.Println(op)
    fmt.Println(addr)
  })
}



