package main

import (
  "harp"
  "encoding/json"
  "flag"
  "io/ioutil"
)


func parseFlags(confPath *string) {
  flag.StringVar(confPath, "conf", "", "path to the config file")
  flag.Parse()
}

func fillConfFromFile(conf *harp.HarpConfig, path string) (err error) {
  var buf []byte
  buf, err = ioutil.ReadFile(path)
  if err != nil {
    return
  }
  err = json.Unmarshal(buf, conf)
  if err != nil {
    return
  }
  return nil
}


func main() {
  var confPath string
  parseFlags(&confPath)
  var conf harp.HarpConfig
  err := fillConfFromFile(&conf, confPath)
  if err != nil {
    return
  }
  server, err := harp.FromConfig(&conf)
  if err != nil {
    return
  }
  server.Start()
}

