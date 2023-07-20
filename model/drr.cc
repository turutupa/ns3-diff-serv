#include "drr.h"

#include "traffic-class.h"

#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("Drr");
NS_OBJECT_ENSURE_REGISTERED(Drr);

TypeId Drr::GetTypeId(void) {
  static TypeId tid = TypeId("ns3::Drr")
                          .SetParent<DiffServ>()
                          .SetGroupName("DiffServ")
                          .AddConstructor<Drr>();
  return tid;
}

Drr::Drr()
    : deficits(std::vector<uint32_t>()), activeList(std::queue<int>()),
      activeMembers(std::set<int>()), isActive(false) {}

Drr::~Drr() {}

bool Drr::Enqueue(Ptr<Packet> packet) {
  int queueInd = Classify(packet);
  bool isEnqueued = DoEnqueue(packet);
  // If enqueuing failed, go about your business
  if (!isEnqueued) {
    return isEnqueued;
  }
  bool isInList = activeMembers.find(queueInd) != activeMembers.end();
  if (isInList) {
    return isEnqueued;
  }
  deficits.reserve(queueInd);
  deficits[queueInd] = 0;
  activeMembers.insert(queueInd);
  activeList.push(queueInd);
  return isEnqueued;
}

Ptr<Packet> Drr::Dequeue() {
  if (activeList.empty()) {
    return nullptr;
  }
  while (true) {
    int ind = activeList.front();
    deficits.reserve(ind);
    // Add to deficit counter if new queue, but only once per queue
    if (!isActive) {
      // Increase queue's deficit counter
      deficits[ind] += (uint32_t)q_class[ind]->GetWeight();
      isActive = true;
    }
    // Packet too large, move to next queue and push to back of line
    if (q_class[ind]->GetBytes() > deficits[ind]) {
      activeList.pop();
      activeList.push(ind);
      isActive = false;
      continue;
    }
    Ptr<Packet> pkt = q_class[ind]->Dequeue();
    deficits[ind] -= pkt->GetSize();
    // Queue empty, reset deficit and remove from active
    if (q_class[ind]->GetPackets() == 0) {
      deficits[ind] = 0;
      activeList.pop();
      activeMembers.erase(ind);
    }
    return pkt;
  }
}

void Drr::SetTrafficClasses(std::vector<TrafficClass *> trafficClasses) {
  deficits.reserve(trafficClasses.size());
  for (uint32_t i = 0; i < trafficClasses.size(); ++i) {
    TrafficClass *traffic = trafficClasses[i];
    TrafficClass *new_traffic = new TrafficClass(*traffic);
    q_class.push_back(new_traffic);
  }
}

Ptr<Packet> Drr::Remove() {
  // Todo: Do we even need to implement this?
  return 0;
}

Ptr<Packet> Drr::Schedule() {
  // Todo
  return 0;
}

Ptr<const Packet> Drr::Peek() const {
  // Todo: Do we need this?
  return 0;
}

} // namespace ns3
