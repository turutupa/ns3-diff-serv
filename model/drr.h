#ifndef DRR_H
#define DRR_H

#include "diff-serv.h"

#include <string>
#include <vector>

namespace ns3 {

class Drr : public DiffServ {
public:
  static TypeId GetTypeId(void);
  Drr();
  ~Drr();

  bool Enqueue(Ptr<Packet>) override;
  Ptr<Packet> Dequeue() override;
  Ptr<const Packet> Peek() const override;
  Ptr<Packet> Remove() override;
  void SetTrafficClasses(std::vector<TrafficClass *>);
  Ptr<Packet> Schedule() override;

private:
  std::vector<uint32_t> deficits;
  std::queue<int> activeList;
  std::set<int> activeMembers;
  bool isActive;

  using DiffServ::Classify;
  using DiffServ::DoDequeue;
  using DiffServ::DoEnqueue;
};

} // namespace ns3

#endif /* DRR_H */
