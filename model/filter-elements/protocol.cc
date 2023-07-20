#include "protocol.h"

#include "ns3/ipv4-header.h"
#include "ns3/ppp-header.h"

namespace ns3 {

Protocol::Protocol(uint8_t value) : value{value} {}

Protocol::~Protocol() {}

bool Protocol::match(const Ptr<ns3::Packet> p) {
  // remove headers
  PppHeader ppp;
  p->RemoveHeader(ppp);

  Ipv4Header ipHeader;
  if (p->PeekHeader(ipHeader) == 0) {
    return false;
  };
  uint8_t protocol = ipHeader.GetProtocol();

  // add header
  p->AddHeader(ppp);

  return protocol == this->value;
}

} // namespace ns3
