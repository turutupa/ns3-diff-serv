#ifndef FILTER_ELEMENT_H
#define FILTER_ELEMENT_H

#include "ns3/packet.h"

namespace ns3 {

const uint8_t TCP_PROTOCOL = 6;
const uint8_t UDP_PROTOCOL = 17;

class FilterElement {
public:
  virtual bool match(const Ptr<ns3::Packet> p) = 0;
};

} // namespace ns3

#endif
