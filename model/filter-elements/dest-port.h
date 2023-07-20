#ifndef DEST_PORT_H
#define DEST_PORT_H

#include "filter-element.h"

namespace ns3 {

class DestPort : public FilterElement {
public:
  DestPort(uint16_t value);
  ~DestPort();

  bool match(const Ptr<ns3::Packet> p) override;

private:
  uint32_t value;
};

} // namespace ns3

#endif
