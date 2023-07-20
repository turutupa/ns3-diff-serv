#include "spq.h"

#include "./filter-elements/dest-ip-mask.h"
#include "./filter-elements/dest-port.h"
#include "./filter-elements/protocol.h"
#include "./filter-elements/source-ip-mask.h"
#include "./filter-elements/source-port.h"
#include "./json.hpp"
#include "traffic-class.h"

#include "ns3/abort.h"
#include "ns3/config.h"
#include "ns3/ipv4-header.h"
#include "ns3/log.h"
#include "ns3/traffic-class.h"

#include <fstream>
#include <sstream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("Spq");
NS_OBJECT_ENSURE_REGISTERED(Spq);

TypeId Spq::GetTypeId(void) {
  static TypeId tid = TypeId("ns3::Spq")
                          .SetParent<DiffServ>()
                          .SetGroupName("DiffServ")
                          .AddConstructor<Spq>();
  return tid;
}

Spq::Spq() {}

// this doesn't have my code for ReadConfigFile
Spq::Spq(std::string configPath) {
  std::vector<TrafficClass *> trafficClasses = this->ReadConfigFile(configPath);
  this->SetTrafficClasses(trafficClasses);
}

Spq::~Spq() {}

bool compareByPriorityLevel(TrafficClass *a, TrafficClass *b) {
  return a->GetPriorityLevel() > b->GetPriorityLevel();
}

void Spq::SetTrafficClasses(std::vector<TrafficClass *> trafficClasses) {
  for (uint32_t i = 0; i < trafficClasses.size(); ++i) {
    TrafficClass *traffic = trafficClasses[i];
    TrafficClass *new_traffic = new TrafficClass(*traffic);
    uint32_t priority_level = traffic->GetPriorityLevel();
    m_priorityLevels.push_back(priority_level);
    q_class.push_back(new_traffic);
  }
  // sort from greates to lowest priority level
  std::sort(m_priorityLevels.begin(), m_priorityLevels.end(),
            std::greater<uint32_t>());
}

Ptr<Packet> Spq::Schedule() {
  for (uint32_t priority_level : m_priorityLevels) {
    for (TrafficClass *trafficClass : q_class) {
      if (trafficClass->GetPriorityLevel() != priority_level)
        continue;

      uint32_t num_packets = trafficClass->GetPackets();
      if (num_packets > 0) {
        return trafficClass->Dequeue();
      }
    }
  }
  return nullptr;
}

Ptr<Packet> Spq::DoRemove() {
  // Implement packet removal logic here
  return 0;
}

Ptr<const Packet> Spq::DoPeek() { return nullptr; }

bool Spq::Enqueue(ns3::Ptr<ns3::Packet> packet) { return DoEnqueue(packet); }

ns3::Ptr<ns3::Packet> Spq::Dequeue() { return Schedule(); }

ns3::Ptr<ns3::Packet> Spq::Remove() { return DoRemove(); }

Ptr<const Packet> Spq::Peek() const { return nullptr; }

} // namespace ns3
