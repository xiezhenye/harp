package config

import "testing"

func TestFile(t *testing.T) {
    conf, err:= NewFileConfig("./test.config")
    if err != nil {
        t.Errorf("create failed %s", err)
    }
    if conf.GetMacByIp("192.168.1.123") != "11:22:33:44:55:66" {
        t.Fail()
    }
    if conf.GetMacByIp("10.0.123.123") != "00:aa:bb:cc:dd:ee" {
        t.Fail()
    }
    if conf.GetMacByIp("8.8.8.8") != "" {
        t.Fail()
    }
}
