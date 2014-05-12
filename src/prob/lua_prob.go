package prob

import (
  "github.com/aarzilli/golua/lua"
  "fmt"
)

type LuaProb struct {
  lua    *lua.State
  name   string
}

func NewLuaProb(dir string, name string) *LuaProb {
  ret := new(LuaProb)
  ret.lua = lua.NewState()
  ret.lua.OpenLibs()
  path := dir + "/" + name + ".lua"
  err := ret.lua.DoFile(path)
  if err != nil {
    return nil
  }
  ret.lua.GetGlobal(name)
  if !ret.lua.IsFunction(-1) {
    fmt.Println(name + " is not function")
    return nil
  }
  ret.lua.Pop(1) 
  ret.name = name
  return ret
}

func (self *LuaProb) Test(attr map[string]string) int {
  self.lua.GetGlobal(self.name)
  self.lua.NewTable()
  for k, v := range attr {
    self.lua.PushString(v)
    self.lua.SetField(-2, k)
  }
  self.lua.Call(1, 1)
  ret := self.lua.OptInteger(-1, -1)
  self.lua.Pop(1)
  return ret
}

func (self *LuaProb) Close() {
  self.lua.Close()
}

