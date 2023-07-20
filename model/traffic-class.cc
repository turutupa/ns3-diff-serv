#include "./traffic-class.h"

#include "filter.h"

#include "ns3/packet.h"

#include <queue>
#include <vector>

namespace ns3 {

TrafficClass::TrafficClass()
    : m_bytes(0), m_packets(0), m_maxPackets(0), m_maxBytes(0), m_weight(0),
      m_priority_level(0), m_isDefault(false) {}

TrafficClass::TrafficClass(uint32_t bytes, uint32_t packets,
                           uint32_t maxPackets, uint32_t maxBytes,
                           double weight, uint32_t priority_level,
                           bool isDefault)
    : m_bytes(bytes), m_packets(packets), m_maxPackets(maxPackets),
      m_maxBytes(maxBytes), m_weight(weight), m_priority_level(priority_level),
      m_isDefault(isDefault) {}

TrafficClass::~TrafficClass() {}

bool TrafficClass::Enqueue(Ptr<Packet> p) {
  if (m_packets >= m_maxPackets || m_bytes + p->GetSize() > m_maxBytes) {
    return false;
  }
  m_queue.push(p);
  m_packets++;
  m_bytes += p->GetSize();
  return true;
}

Ptr<Packet> TrafficClass::Dequeue() {
  if (m_queue.empty()) {
    return 0;
  }
  Ptr<Packet> p = m_queue.front();
  m_queue.pop();
  m_packets--;
  m_bytes -= p->GetSize();
  return p;
}

bool TrafficClass::Match(Ptr<Packet> p) {
  if (m_isDefault) {
    return true;
  }
  for (Filter *filter : filters) {
    if (filter->match(p)) {
      return true;
    }
  }
  return false;
}

uint32_t TrafficClass::GetBytes() { return m_bytes; }

void TrafficClass::SetBytes(uint32_t bytes) { m_bytes = bytes; }

uint32_t TrafficClass::GetPackets() { return m_packets; }

void TrafficClass::SetPackets(uint32_t packets) { m_packets = packets; }

uint32_t TrafficClass::GetMaxPackets() { return m_maxPackets; }

void TrafficClass::SetMaxPackets(uint32_t maxPackets) {
  m_maxPackets = maxPackets;
}

uint32_t TrafficClass::GetMaxBytes() { return m_maxBytes; }

void TrafficClass::SetMaxBytes(uint32_t maxBytes) { m_maxBytes = maxBytes; }

double TrafficClass::GetWeight() { return m_weight; }

void TrafficClass::SetWeight(double weight) { m_weight = weight; }

uint32_t TrafficClass::GetPriorityLevel() { return m_priority_level; }

void TrafficClass::SetPriorityLevel(uint32_t level) {
  m_priority_level = level;
}

bool TrafficClass::IsDefault() { return m_isDefault; }

void TrafficClass::SetIsDefault(bool isDefault) { m_isDefault = isDefault; }

} // namespace ns3
