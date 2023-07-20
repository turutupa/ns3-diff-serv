#include "dest-ip-mask.h"

#include "ns3/ipv4-header.h"
#include "ns3/ppp-header.h"

namespace ns3 {

DestIpMask::DestIpMask(const Ipv4Address a, const Ipv4Mask m)
    : address{a}, mask{m} {}

DestIpMask::~DestIpMask() {}

bool DestIpMask::match(const Ptr<ns3::Packet> p) {
  // remove headers
  PppHeader ppp;
  p->RemoveHeader(ppp);
  Ipv4Header hdr;
  p->PeekHeader(hdr);

  Ipv4Address dstIp = hdr.GetDestination();

  // add headers
  p->AddHeader(ppp);

  return (this->mask.IsMatch(this->address, dstIp));
}

} // namespace ns3
