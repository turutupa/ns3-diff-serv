#include "source-ip-mask.h"

#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/ppp-header.h"

namespace ns3 {

SourceIpMask::SourceIpMask(const Ipv4Address a, const Ipv4Mask m)
    : address{a}, mask{m} {}

SourceIpMask::~SourceIpMask() {}

bool SourceIpMask::match(const Ptr<ns3::Packet> p) {
  // remove headers
  PppHeader ppp;
  p->RemoveHeader(ppp);
  Ipv4Header hdr;
  p->PeekHeader(hdr);
  Ipv4Address srcIp = hdr.GetSource();

  // add headers
  p->AddHeader(ppp);

  return (this->mask.IsMatch(this->address, srcIp));
}

} // namespace ns3
