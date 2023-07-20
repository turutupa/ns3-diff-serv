#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "filter-element.h"

namespace ns3 {

class Protocol : public FilterElement {
public:
  Protocol(uint8_t value);
  ~Protocol();

  virtual bool match(const Ptr<ns3::Packet> p) override;

private:
  uint32_t value;
};

} // namespace ns3

#endif
