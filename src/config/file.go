package config

import (
"encoding/json"
"io/ioutil"
"sync"
)

type FileConfig struct {
    path string
    data map[string]string
    callback OnUpdateCallback
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
    return
}

func (self *FileConfig) Reload() (err error) {
    content, err := ioutil.ReadFile(self.path)
    if err != nil {
        return
    }
    self.lock.Lock()
    defer self.lock.Unlock()
    err = json.Unmarshal(content, &self.data)
    return
}
