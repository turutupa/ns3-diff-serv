#ifndef SOURCE_PORT_H
#define SOURCE_PORT_H

#include "filter-element.h"

namespace ns3 {

class SourcePort : public FilterElement {
public:
  SourcePort(uint32_t value);
  ~SourcePort();

  bool match(Ptr<ns3::Packet> p) override;

private:
  uint32_t value;
};

} // namespace ns3

#endif
