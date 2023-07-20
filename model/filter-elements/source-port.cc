#include "source-port.h"

#include "ns3/ipv4-header.h"
#include "ns3/ppp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"

namespace ns3 {

SourcePort::SourcePort(uint32_t value) : value{value} {}

SourcePort::~SourcePort() {}

bool SourcePort::match(const Ptr<ns3::Packet> p) {
  bool isMatch;
  PppHeader ppp;
  p->RemoveHeader(ppp);
  Ipv4Header hdr;
  p->RemoveHeader(hdr);

  uint8_t protocol = hdr.GetProtocol();
  uint8_t srcPort;
  if (protocol == TCP_PROTOCOL) {
    TcpHeader tcpHdr;
    p->PeekHeader(tcpHdr);
    srcPort = tcpHdr.GetSourcePort();
    isMatch = srcPort == this->value;
  } else if (protocol == UDP_PROTOCOL) {
    UdpHeader udpHdr;
    p->PeekHeader(udpHdr);
    srcPort = udpHdr.GetSourcePort();
    isMatch = srcPort == this->value;
  }
  p->AddHeader(hdr);
  p->AddHeader(ppp);

  return isMatch;
}

} // namespace ns3
