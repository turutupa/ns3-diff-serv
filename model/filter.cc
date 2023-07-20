#include "ns3/filter.h"

#include "ns3/ipv4-header.h"
#include "ns3/ppp-header.h"
#include "ns3/udp-header.h"

namespace ns3 {

Filter::Filter() {}

Filter::~Filter() {}

bool Filter::match(Ptr<ns3::Packet> p) {
  for (FilterElement *element : elements) {
    if (!element->match(p)) {
      return false;
    }
  }
  return true;
}

} // namespace ns3
