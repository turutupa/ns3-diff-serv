#ifndef FILTER_H
#define FILTER_H

#include "filter-elements/filter-element.h"

#include "ns3/packet.h"

#include <vector>

namespace ns3 {

class Filter {
public:
  Filter();
  ~Filter();

  std::vector<FilterElement *> elements;

  bool match(Ptr<ns3::Packet> p);
};

} // namespace ns3

#endif
