#include "dest-port.h"

#include "ns3/ipv4-header.h"
#include "ns3/ppp-header.h"
#include "ns3/tcp-header.h"
#include "ns3/udp-header.h"

namespace ns3 {

DestPort::DestPort(uint16_t value) : value{value} {}

DestPort::~DestPort() {}

bool DestPort::match(const Ptr<ns3::Packet> p) {
  bool isMatch = false;
  // remove headers
  PppHeader ppp;
  p->RemoveHeader(ppp);
  Ipv4Header ip;
  p->RemoveHeader(ip);

  uint32_t protocol = ip.GetProtocol();
  if (protocol == UDP_PROTOCOL) {
    UdpHeader udp;
    p->PeekHeader(udp);
    isMatch = (this->value == udp.GetDestinationPort());
  } else if (protocol == TCP_PROTOCOL) {
    TcpHeader tcp;
    p->PeekHeader(tcp);
    isMatch = (this->value == tcp.GetDestinationPort());
  }

  // add headers
  p->AddHeader(ip);
  p->AddHeader(ppp);

  return isMatch;
}

} // namespace ns3
