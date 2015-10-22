package config

import (
"time"
//"github.com/coreos/etcd/Godeps/_workspace/src/golang.org/x/net/context"
"github.com/coreos/etcd/client"
"sync"
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
    /*
    err := self.kapi.Create("")
    if err == nil {
        self.setIsActive(true)
        //keep update
        for {
            err = self.kapi.Update("")
            //sleep
        }
    } else {
        self.setIsActive(false)
        for {
            //watch?grab?
        }
    }
    */
}

func (self *EtcdConfig) setIsActive(b bool) {
    self.lock.Lock()
    self.isActive = b
    self.lock.Unlock()
}

func (self *EtcdConfig) IsActive(b bool) {
    self.lock.RLock()
    b = self.isActive
    self.lock.RUnlock()
}

func (self *EtcdConfig) watchForUpdate() {
     // watch data, update cache
}

