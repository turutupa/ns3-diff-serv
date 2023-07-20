#ifndef SOURCE_IP_MASK_H
#define SOURCE_IP_MASK_H

#include "filter-element.h"

#include "ns3/ipv4-address.h"

namespace ns3 {

class SourceIpMask : public FilterElement {
public:
  SourceIpMask(const Ipv4Address a, const Ipv4Mask m);
  ~SourceIpMask();

  bool match(const Ptr<ns3::Packet> p) override;

private:
  Ipv4Address address;
  Ipv4Mask mask;
};

} // namespace ns3

#endif
