package config

import (
"encoding/json"
"io/ioutil"
"sync"
"os"
"os/signal"
"syscall"
"fmt"
)

type FileConfig struct {
    path string
    data map[string]string
    callback OnChangeCallback
    lock sync.RWMutex
}

func (self *FileConfig) GetMacByIp(ip string) string {
    self.lock.RLock()
    mac, ok := self.data[ip]
    if !ok {
        mac = ""
    }
    self.lock.RUnlock()
    return mac
}

func (self *FileConfig) SetOnChange(callback OnChangeCallback) {
    self.callback = callback
}

func (self *FileConfig) Path() string {
    return self.path
}

func NewFileConfig(path string) (ret *FileConfig, err error) {
    ret = new(FileConfig)
    ret.path = path
    err = ret.Reload()
    go ret.SignalHandler()
    return
}

func (self *FileConfig) Reload() (err error) {
    content, err := ioutil.ReadFile(self.path)
    if err != nil {
        return
    }
    self.lock.Lock()
    defer self.lock.Unlock()
    t := make(map[string]string)
    err = json.Unmarshal(content, &t)
    if err != nil {
        return
    }
    self.data = t
    if self.callback != nil {
        for ip, mac := range(self.data) {
            self.callback(ip, mac)
        }
    }
    return
}


func (self *FileConfig) SignalHandler() {
  c := make(chan os.Signal, 1)
  signal.Notify(c, syscall.SIGUSR2)
  for {
    // Block until a signal is received.
    <-c
    err := self.Reload() 
    if err != nil {
      fmt.Println("reload failed")
    } else {
      fmt.Println("reload ok")
    }
  }
}

