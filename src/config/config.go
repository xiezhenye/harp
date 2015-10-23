package config

import (
    "strings"
    "fmt"
)


type OnChangeCallback func(ip string, mac string)
type Config interface {
    GetMacByIp(ip string) (mac string)
    SetOnChange(callback OnChangeCallback)
}


// file:/xxx/xxx/xxx.conf
// etcd:node1@123.123.1.1:111,123.123.1.2:111/harp
func NewConfig(desc string) (ret Config, err error) {
    secs := strings.SplitN(desc, ":", 2)
    if len(secs) != 2 {
        err = fmt.Errorf("bad descriptor format")
        return
    }
    class:= secs[0]
    switch class {
    case "file":
        ret, err = NewFileConfig(secs[1])
    case "etcd":
        secs = strings.SplitN(secs[1], "@", 2)
        if len(secs) != 2 {
            err = fmt.Errorf("bad descriptor format")
            return
        }
        nodeName:= secs[0]
        secs = strings.SplitN(secs[1], "/", 2)
        if len(secs) != 2 {
            err = fmt.Errorf("bad descriptor format")
            return
        }
        prefix:= secs[1]
        endpoints:= strings.Split(secs[0], ",")
        for i := range endpoints {
          endpoints[i] = "http://"+endpoints[i]
        }
        ret, err = NewEtcdConfig(nodeName, endpoints, prefix)
    default:
        err = fmt.Errorf("unknown config class")
    }
    return
}

