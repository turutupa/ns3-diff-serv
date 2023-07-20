#ifndef TRAFFIC_CLASS_H
#define TRAFFIC_CLASS_H

#include "./filter.h"

#include "ns3/queue.h"

#include <queue>

namespace ns3 {

class TrafficClass {
public:
  // Constructors and destructor
  TrafficClass();
  TrafficClass(uint32_t bytes, uint32_t packets, uint32_t maxPackets,
               uint32_t maxBytes, double weight, uint32_t priority_level,
               bool isDefault);
  ~TrafficClass();

  // Attributes
  std::vector<ns3::Filter *> filters;

  // Methods
  bool Enqueue(Ptr<ns3::Packet> p);
  Ptr<ns3::Packet> Dequeue();
  bool Match(Ptr<ns3::Packet> p);

  // Setters and getters
  uint32_t GetBytes();
  void SetBytes(uint32_t bytes);
  uint32_t GetPackets();
  void SetPackets(uint32_t packets);
  uint32_t GetMaxPackets();
  void SetMaxPackets(uint32_t maxPackets);
  uint32_t GetMaxBytes();
  void SetMaxBytes(uint32_t maxBytes);
  double GetWeight();
  void SetWeight(double weight);
  uint32_t GetPriorityLevel();
  void SetPriorityLevel(uint32_t level);
  bool IsDefault();
  void SetIsDefault(bool isDefault);

private:
  uint32_t m_bytes;
  uint32_t m_packets;
  uint32_t m_maxPackets;
  uint32_t m_maxBytes;
  double m_weight;
  uint32_t m_priority_level;
  bool m_isDefault;
  std::queue<Ptr<ns3::Packet>> m_queue;
};

} // namespace ns3

#endif
