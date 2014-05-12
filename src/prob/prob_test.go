package prob

import (
  "testing"
)

func TestLua(t *testing.T) {
  prob := NewLuaProb(".", "test")
  if prob == nil {
    return
  }
  defer prob.Close()
  if prob.Test(map[string]string{"foo": "1"}) != 1 {
    t.Fail()
  }
  if prob.Test(map[string]string{"foo": "333"}) != 3 {
    t.Fail()
  }
}

