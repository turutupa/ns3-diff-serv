#ifndef DIFF_SERV_H
#define DIFF_SERV_H

// Add a doxygen group for this module.
// If you have more than one file, this should be in only one of them.
/**
 * \defgroup diff-serv Description of the diff-serv
 */
#include "traffic-class.h"

#include "ns3/queue.h"

#include <vector>

namespace ns3 {

// Each class should be documented using Doxygen,
// and have an \ingroup diff-serv directive
class DiffServ : public Queue<Packet> {
public:
  static TypeId GetTypeId(void);
  virtual Ptr<Packet> Schedule() = 0;
  uint32_t Classify(Ptr<Packet> p);
  ~DiffServ() noexcept override;
  const std::vector<TrafficClass *> &GetQClass() const;
  bool DoEnqueue(Ptr<Packet> p);
  Ptr<Packet> DoDequeue(void);
  std::vector<TrafficClass *> ReadConfigFile(const std::string &filename);

protected:
  // m_mode absent due to QueueMode removed from ns3
  std::vector<TrafficClass *> q_class;
  virtual Ptr<Packet> DoRemove();
  virtual Ptr<const Packet> DoPeek();
};

/* ... */

} // namespace ns3

#endif /* DIFF_SERV_H */
