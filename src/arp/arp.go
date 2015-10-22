package arp

import (
  "syscall"
  "unsafe"
  "net"
//  "fmt"
)

type ArpListener struct {
  sock int
}

func NewListener() (ret ArpListener, err error) {
  ret.sock, err =  syscall.Socket(syscall.AF_PACKET, syscall.SOCK_RAW, int(htons(syscall.ETH_P_ARP)))
  return
}


func (self *ArpListener) Close() {
  syscall.Close(self.sock)
}

type ArpPacket struct {
  DestHwAddr   [6]byte
  SrcHwAddr    [6]byte
  FrameType    uint16
  HwType       uint16
  ProtType     uint16
  HwAddrSize   byte
  ProtAddrSize byte
  Op           uint16
  SndrHwAddr   [6]byte
  SndrIpAddr   [4]byte
  RcptHwAddr   [6]byte
  RcptIpAddr   [4]byte
  padding      [18]byte
}

const (
  ARP_REQUEST = 1
  ARP_REPLY   = 2
)

type ArpOp struct {
  Op         uint16
  SndrHwAddr net.HardwareAddr 
  SndrIpAddr net.IP
  RcptHwAddr net.HardwareAddr
  RcptIpAddr net.IP
}

func htons(n uint16) uint16 {  
  var (  
    high uint16 = n >> 8  
    ret  uint16 = n << 8 + high  
  )  
  return ret  
}

func (self *ArpPacket) ArpOp() ArpOp {
  var ret ArpOp
  ret.Op = htons(self.Op)
  ret.SndrHwAddr = net.HardwareAddr(self.SndrHwAddr[:])
  ret.SndrIpAddr = net.IP(self.SndrIpAddr[:])
  ret.RcptHwAddr = net.HardwareAddr(self.RcptHwAddr[:])
  ret.RcptIpAddr = net.IP(self.RcptIpAddr[:])
  return ret
}

func (self *ArpOp) ArpPacket() ArpPacket {
  var ret ArpPacket
  ret.FrameType    = htons(syscall.ETH_P_ARP)
  ret.HwType       = htons(syscall.ARPHRD_ETHER)
  ret.ProtType     = htons(syscall.ETH_P_IP)
  ret.HwAddrSize   = 6
  ret.ProtAddrSize = 4
  ret.Op           = htons(self.Op)
  copy(ret.DestHwAddr[:], self.RcptHwAddr)
  copy(ret.SrcHwAddr[:],  self.SndrHwAddr)
  copy(ret.SndrHwAddr[:], self.SndrHwAddr)
  copy(ret.RcptHwAddr[:], self.RcptHwAddr)
  copy(ret.SndrIpAddr[:], self.SndrIpAddr)
  copy(ret.RcptIpAddr[:], self.RcptIpAddr)
  return ret
}

func GratuitousArpOp(ip net.IP, hwAddr net.HardwareAddr) ArpOp {
  var ret ArpOp
  ret.Op = ARP_REQUEST
  ret.SndrIpAddr = ip
  ret.RcptIpAddr = ip
  ret.SndrHwAddr = hwAddr
  ret.RcptHwAddr, _ = net.ParseMAC("ff:ff:ff:ff:ff:ff")
  return ret
}

type Addr syscall.Sockaddr

func AddrFromName(name string) (Addr, error) {
  ifc, err := net.InterfaceByName(name)
  if err != nil {
    return nil, err
  }
  ret := AddrFromInterface(ifc)
  return ret, nil
}

func AddrFromInterface(ifc *net.Interface) Addr {
  return &syscall.SockaddrLinklayer{Protocol: htons(syscall.ETH_P_ARP), Ifindex: ifc.Index} 
}

type ArpReciever func(ArpOp, Addr)

func packetBuf(packet *ArpPacket) []byte {
  var sls struct {  
    addr uintptr
    len  int
    cap  int
  }
  size := int(unsafe.Sizeof(*packet))
  sls.addr = uintptr(unsafe.Pointer(packet))
  sls.len = size
  sls.cap = size
  return *(*[]byte)(unsafe.Pointer(&sls))
}

func (self *ArpListener) Listen(reciever ArpReciever) {
  var packet ArpPacket
  buf := packetBuf(&packet) 
  for { 
    _, from, err := syscall.Recvfrom(self.sock, buf, 0)
    if err != nil {
      //TODO: is recoverable?
      return
    }
    //fmt.Println("in:")
    //fmt.Println(packet)
    reciever(packet.ArpOp(), from)
  }
}

func (self *ArpListener) Send(op ArpOp, addr Addr) error {
  packet := op.ArpPacket()
  //fmt.Println("out:")
  //fmt.Println(packet)
  buf := packetBuf(&packet)
  ifi, err := net.InterfaceByIndex(addr.(*syscall.SockaddrLinklayer).Ifindex)
  //ifi, err := net.InterfaceByName("eth1")
  if err != nil {
    return err
  }
  lla := syscall.SockaddrLinklayer{Protocol: htons(syscall.ETH_P_ARP), Ifindex: ifi.Index}
  return syscall.Sendto(self.sock, buf, 0, &lla)

}

