#ifndef SPQ_H
#define SPQ_H

#include "./diff-serv.h"

#include <string>
#include <vector>

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup spq Description of the spq
 */

namespace ns3 {

// Each class should be documented using Doxygen,
// and have an \ingroup spq directive

class Spq : public DiffServ {
public:
  static TypeId GetTypeId(void);
  Spq();
  Spq(std::string configPath);
  ~Spq();

  void SetTrafficClasses(std::vector<TrafficClass *> traffic_classes);
  Ptr<Packet> Schedule() override;

  virtual bool Enqueue(ns3::Ptr<ns3::Packet> item) override;
  virtual ns3::Ptr<ns3::Packet> Dequeue() override;
  virtual ns3::Ptr<ns3::Packet> Remove() override;
  virtual ns3::Ptr<const ns3::Packet> Peek() const override;

protected:
  Ptr<Packet> DoRemove() override;
  Ptr<const Packet> DoPeek() override;

private:
  std::vector<uint32_t> m_priorityLevels;
};

} // namespace ns3

#endif /* SPQ_H */
