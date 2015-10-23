package config

import (
"time"
"github.com/coreos/etcd/Godeps/_workspace/src/golang.org/x/net/context"
"github.com/coreos/etcd/client"
"sync"
"fmt"
)

const (
    activeNodeKey     = "/activeNode"
    dataKey           = "/data"
    heartbeatTimeout  = 3
    heartbeatInterval = 1
)

type EtcdConfig struct {
    nodeId    string
    isActive  bool
    endpoints []string
    prefix    string
    kapi      client.KeysAPI
    client    client.Client
    cache     map[string]string
    lock      sync.RWMutex
}

func NewEtcdConfig(nodeId string, endpoints []string, prefix string) (ret *EtcdConfig, err error) {
    ret = new(EtcdConfig)
    ret.endpoints = endpoints
    ret.prefix = prefix
    cfg := client.Config{
        Endpoints:               ret.endpoints,
        Transport:               client.DefaultTransport,
        // set timeout per request to fail fast when the target endpoint is unavailable
        HeaderTimeoutPerRequest: time.Second,
    }
    ret.isActive = false
    ret.client, err = client.New(cfg)
    if err != nil {
        return
    }
    ret.kapi = client.NewKeysAPI(ret.client)
    go ret.keepalive()
    go ret.watchForUpdate()
    return
}

func (self *EtcdConfig) GetMacByIp(ip string) string {
    self.lock.RLock()
    defer self.lock.RUnlock()
    if !self.isActive {
        return ""        
    }
    mac, ok := self.cache[ip]
    if !ok {
        return ""
    }
    return mac
}

func (self *EtcdConfig) keepalive() {
    grabOption:= client.SetOptions{
      PrevExist: client.PrevNoExist,
      TTL: time.Second * heartbeatTimeout,
    }
    heartbeatOption:= client.SetOptions{
      TTL: time.Second * heartbeatTimeout,
    }
    for {
      var opt *client.SetOptions
      if self.IsActive() {
        opt = &heartbeatOption
      } else { 
        opt = &grabOption
      }
      _, err := self.kapi.Set(context.Background(), self.prefix+activeNodeKey, self.nodeId, opt)
      self.setIsActive(err == nil)
      switch err := err.(type) {
      case *client.Error:
      case nil:
      default:
        //TODO: unknown error!
        fmt.Println(err)
      }
      time.Sleep(heartbeatInterval * time.Second)
    }
 }

func (self *EtcdConfig) setIsActive(b bool) {
    self.lock.Lock()
    self.isActive = b
    self.lock.Unlock()
}

func (self *EtcdConfig) IsActive() (b bool) {
    self.lock.RLock()
    b = self.isActive
    self.lock.RUnlock()
    return
}

func (self *EtcdConfig) watchForUpdate() {
  //fetch
  resp, err := self.kapi.Get(context.Background(), self.prefix+dataKey, nil)
  if err != nil {
    // exit
    return
  }
  fmt.Println(resp)
  // watch data, update cache
}

