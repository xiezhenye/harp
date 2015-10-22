package config

import (
    "strings"
    "fmt"
)

type Config interface {
    GetMacByIp(ip string) (mac string)
    //SetOnChange(callback func(ip string, mac string))
}

// file:/xxx/xxx/xxx.conf
// etcd:node1@123.123.1.1:111,123.123.1.2:111/harp
func NewConfig(desc string) (ret Config, err error) {
    secs := strings.SplitN(desc, ":", 2)
    if len(secs) != 2 {
        err = fmt.Errorf("bad descriptor format")
        return
    }
    class:= secs[1]
    switch class {
    case "file":
        ret, err = NewFileConfig(secs[1])
    case "etcd":
        secs = strings.SplitN(secs[1], "/", 2)
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
        ret, err = NewEtcdConfig(nodeName, endpoints, prefix)
    default:
        err = fmt.Errorf("unknown config class")
    }
    return
}
