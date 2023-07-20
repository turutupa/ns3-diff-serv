#include "diff-serv.h"

#include "./filter-elements/dest-ip-mask.h"
#include "./filter-elements/dest-port.h"
#include "./filter-elements/protocol.h"
#include "./filter-elements/source-ip-mask.h"
#include "./filter-elements/source-port.h"
#include "./json.hpp"

#include "ns3/ipv4-header.h"
#include "ns3/log.h"
#include "ns3/ppp-header.h"
#include "ns3/udp-header.h"

#include <fstream>
#include <iostream>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE("DiffServ");
NS_OBJECT_ENSURE_REGISTERED(DiffServ);

DiffServ::~DiffServ() noexcept {
  for (auto traffic_class : q_class) {
    delete traffic_class;
  }
}

TypeId DiffServ::GetTypeId(void) {
  static TypeId tid =
      TypeId("ns3::DiffServ").SetParent<Object>().SetGroupName("DiffServ");
  return tid;
}

const std::vector<TrafficClass *> &DiffServ::GetQClass() const {
  return q_class;
}

Ptr<Packet> DiffServ::Schedule() {
  // to be implemented by queue algorithm
  return nullptr;
}

uint32_t DiffServ::Classify(Ptr<Packet> p) {
  int defaultTrafficClass = -1;
  uint32_t numQueues = q_class.size();
  for (uint32_t i = 0; i < numQueues; ++i) {
    TrafficClass *trafficClass = q_class[i];
    if (trafficClass->Match(p)) {
      return i;
    }
    if (trafficClass->IsDefault()) {
      defaultTrafficClass = i;
    }
  }
  if (defaultTrafficClass >= 0)
    return defaultTrafficClass;
  return q_class.size() - 1;
}

bool DiffServ::DoEnqueue(Ptr<Packet> p) {
  uint32_t classIndex = Classify(p);
  Ipv4Header ipHeader;
  p->PeekHeader(ipHeader);
  return q_class[classIndex]->Enqueue(p);
}

Ptr<Packet> DiffServ::DoDequeue(void) {
  Ptr<Packet> packet = Schedule();
  return packet;
}

Ptr<Packet> DiffServ::DoRemove() {
  // Implement packet removal logic here
  return nullptr;
}

Ptr<const Packet> DiffServ::DoPeek() {
  // Implement packet peeking logic here
  return nullptr;
}

std::vector<TrafficClass *>
DiffServ::ReadConfigFile(const std::string &filename) {
  std::ifstream configFile(filename);
  std::vector<TrafficClass *> trafficClasses;

  if (!configFile.is_open()) {
    std::cerr << "Unable to open config file: " << filename << std::endl;
    return trafficClasses;
  }

  nlohmann::json configJson;
  configFile >> configJson;

  for (const auto &tcJson : configJson["trafficClasses"]) {
    uint32_t bytes = tcJson["bytes"];
    uint32_t packets = tcJson["packets"];
    uint32_t maxPackets = tcJson["maxPackets"];
    uint32_t maxBytes = tcJson["maxBytes"];
    double weight = tcJson["weight"];
    uint32_t priorityLevel = tcJson["priorityLevel"];
    bool isDefault = tcJson["isDefault"];

    TrafficClass *tc = new TrafficClass(bytes, packets, maxPackets, maxBytes,
                                        weight, priorityLevel, isDefault);

    for (const auto &filterArrayJson : tcJson["filters"]) {
      Filter *filter = new Filter();
      for (const auto &filterJson : filterArrayJson) {
        std::string filterType = filterJson["type"];
        FilterElement *filterElement = nullptr;

        if (filterType == "Protocol") {
          uint8_t value = filterJson["value"];
          filterElement = new Protocol(value);
        } else if (filterType == "SourceIpMask") {
          Ipv4Address address(filterJson["address"].get<std::string>().c_str());
          Ipv4Mask mask(filterJson["mask"].get<std::string>().c_str());
          filterElement = new SourceIpMask(address, mask);
        } else if (filterType == "DestIpMask") {
          Ipv4Address address(filterJson["address"].get<std::string>().c_str());
          Ipv4Mask mask(filterJson["mask"].get<std::string>().c_str());
          filterElement = new DestIpMask(address, mask);
        } else if (filterType == "SourcePort") {
          uint32_t value = filterJson["value"];
          filterElement = new SourcePort(value);
        } else if (filterType == "DestPort") {
          uint16_t value = filterJson["value"];
          filterElement = new DestPort(value);
        }
        // Add more filter types here if needed

        if (filterElement) {
          filter->elements.push_back(filterElement);
        }
      }
      tc->filters.push_back(filter);
    }
    trafficClasses.push_back(tc);
  }
  configFile.close();
  return trafficClasses;
}

} // namespace ns3
