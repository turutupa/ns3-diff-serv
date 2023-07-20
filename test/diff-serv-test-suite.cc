#include "../model/diff-serv.h"
#include "../model/filter-elements/dest-ip-mask.h"
#include "../model/filter-elements/dest-port.h"
#include "../model/filter-elements/filter-element.h"
#include "../model/filter-elements/protocol.h"
#include "../model/filter-elements/source-ip-mask.h"
#include "../model/filter-elements/source-port.h"
#include "../model/filter.h"
#include "../model/spq.h"

#include "ns3/ipv4-address.h"
#include "ns3/ipv4-header.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/tcp-header.h"
#include "ns3/test.h"
#include "ns3/udp-header.h"

// An essential include is test.h
#include "ns3/test.h"

// Do not put your test classes in namespace ns3.  You may find it useful
// to use the using directive to access the ns3 namespace directly
using namespace ns3;

// Add a doxygen group for tests.
// If you have more than one test, this should be in only one of them.
/**
 * \defgroup diff-serv-tests Tests for diff-serv
 * \ingroup diff-serv
 * \ingroup tests
 */

/**
 * SPQ Test Case
 */
class SpqTestCase : public TestCase {
public:
  SpqTestCase() : TestCase("SPQ Test Case") {}

  virtual void DoRun() { TestSpq(); }

  void TestSpq() {
    Spq spq;
    FilterElement *tcpProtocolFilter =
        new Protocol(TCP_PROTOCOL); // high priority
    FilterElement *udpProtocolFilter =
        new Protocol(UDP_PROTOCOL); // low priority
    Filter tcpFilter;               // high priority
    Filter udpFilter;               // low priority
    TrafficClass t_high;
    TrafficClass t_low;

    // high priority tcp filter
    tcpFilter.elements.push_back(tcpProtocolFilter);
    t_high.filters.push_back(&tcpFilter);
    t_high.SetPriorityLevel(2);
    t_high.SetIsDefault(false);
    t_high.SetMaxBytes(9999);
    t_high.SetMaxPackets(9999);

    // low priority udp filter
    udpFilter.elements.push_back(udpProtocolFilter);
    t_low.filters.push_back(&udpFilter);
    t_low.SetPriorityLevel(1);
    t_low.SetIsDefault(true);
    t_low.SetMaxBytes(9999);
    t_low.SetMaxPackets(9999);

    // create traffic classes
    std::vector<TrafficClass *> trafficClasses = {};
    trafficClasses.push_back(&t_high);
    trafficClasses.push_back(&t_low);

    spq.SetTrafficClasses(trafficClasses);

    // Create packets to send to SPQ
    Ptr<Packet> packet1 = Create<Packet>();
    Ptr<Packet> packet2 = Create<Packet>();

    Ipv4Header ipv4Header1;
    Ipv4Header ipv4Header2;

    ipv4Header1.SetProtocol(UDP_PROTOCOL); // Lower priority
    ipv4Header2.SetProtocol(TCP_PROTOCOL); // Higher priority

    packet1->AddHeader(ipv4Header1);
    packet2->AddHeader(ipv4Header2);

    spq.Enqueue(packet1);
    spq.Enqueue(packet2);

    Ptr<Packet> scheduledPacket1 = spq.Dequeue();
    Ptr<Packet> scheduledPacket2 = spq.Dequeue();

    // The higher priority packet (packet2) should be scheduled before the lower
    // priority (packet1)
    NS_TEST_ASSERT_MSG_EQ(scheduledPacket1, packet2,
                          "Higher priority packet should be scheduled first");
    NS_TEST_ASSERT_MSG_EQ(scheduledPacket2, packet1,
                          "Lower priority packet should be scheduled second");
  }
};

class ReadConfigFileTestCase : public TestCase {
public:
  ReadConfigFileTestCase() : TestCase("ReadConfigFile Test Case") {}

  virtual void DoRun() {
    testFilters();
    testPort();
  }

  virtual void testFilters() {
    Spq spq;
    std::string filename = "contrib/diff-serv/test/config.json";
    std::vector<TrafficClass *> trafficClasses = spq.ReadConfigFile(filename);

    // Assert that the right number of TrafficClasses were created
    NS_TEST_ASSERT_MSG_EQ(trafficClasses.size(), 2,
                          "Number of TrafficClasses does not match expected");

    // Check properties of the first TrafficClass
    TrafficClass *tc1 = trafficClasses[0];
    NS_TEST_ASSERT_MSG_EQ(
        tc1->GetPriorityLevel(), 2,
        "Priority level of first TrafficClass does not match expected");
    NS_TEST_ASSERT_MSG_EQ(
        tc1->IsDefault(), false,
        "isDefault of first TrafficClass does not match expected");

    // Check properties of the filters of the first TrafficClass
    std::vector<Filter *> filters1 = tc1->filters;
    NS_TEST_ASSERT_MSG_EQ(
        filters1.size(), 2,
        "Number of Filters in first TrafficClass does not match expected");
    // Add more assertions for the properties of the filters...

    // Repeat checks for the second TrafficClass...
    // Check properties of the first TrafficClass
    TrafficClass *tc2 = trafficClasses[1];
    NS_TEST_ASSERT_MSG_EQ(
        tc2->GetPriorityLevel(), 1,
        "Priority level of first TrafficClass does not match expected");
    NS_TEST_ASSERT_MSG_EQ(
        tc2->IsDefault(), true,
        "isDefault of first TrafficClass does not match expected");

    // Check properties of the filters of the first TrafficClass
    std::vector<Filter *> filters2 = tc2->filters;
    NS_TEST_ASSERT_MSG_EQ(
        filters2.size(), 2,
        "Number of Filters in first TrafficClass does not match expected");

    std::vector<FilterElement *> filterElements = filters2[0]->elements;
    NS_TEST_ASSERT_MSG_EQ(
        filterElements.size(), 2,
        "Number of Filters in first TrafficClass does not match expected");

    // Remember to free the TrafficClass objects to avoid memory leaks
    for (TrafficClass *tc : trafficClasses) {
      delete tc;
    }
  }

  virtual void testPort() {
    Ptr<Packet> p1 = Create<Packet>();
    Ptr<Packet> p2 = Create<Packet>();

    UdpHeader udpH1;
    udpH1.SetDestinationPort(2001);
    p1->AddHeader(udpH1);

    Ipv4Header ipH1;
    ipH1.SetProtocol(UDP_PROTOCOL);
    p1->AddHeader(ipH1);

    UdpHeader udpH2;
    udpH2.SetDestinationPort(2003);
    p2->AddHeader(udpH2);

    Ipv4Header ipH2;
    ipH2.SetProtocol(UDP_PROTOCOL);
    p2->AddHeader(ipH2);

    Spq s;
    std::string configPath = "contrib/diff-serv/examples/config.json";
    std::vector<TrafficClass *> trafficClasses = s.ReadConfigFile(configPath);
    s.SetTrafficClasses(trafficClasses);

    s.Enqueue(p1);
    s.Enqueue(p2);
    Ptr<Packet> exp1 = s.Dequeue();
    Ptr<Packet> exp2 = s.Dequeue();

    NS_TEST_ASSERT_MSG_EQ(exp1, p2,
                          "Expected to dequeue first high priority pkt");
    NS_TEST_ASSERT_MSG_EQ(exp2, p1,
                          "Expected to dequeue second low priority pkt");
  }
};

/**
 * filter test cases
 */
class FilterTestCase : public TestCase {
public:
  FilterTestCase() : TestCase("Filter Test Case") {}

  virtual void DoRun() {
    TestFilter();
    TestFilterElement();
  }

  void TestFilter() {
    Filter filter;
    Ptr<Packet> packet = Create<Packet>();

    NS_TEST_ASSERT_MSG_EQ(filter.match(packet), true,
                          "Empty filter should match any packet");

    FilterElement *protocolFilter = new Protocol(UDP_PROTOCOL);
    filter.elements.push_back(protocolFilter);

    Ipv4Header ipHeader;
    ipHeader.SetProtocol(UDP_PROTOCOL);
    packet->AddHeader(ipHeader);

    NS_TEST_ASSERT_MSG_EQ(filter.match(packet), true,
                          "UDP filter should match UDP packet");

    packet = Create<Packet>();
    ipHeader.SetProtocol(TCP_PROTOCOL);
    packet->AddHeader(ipHeader);
    NS_TEST_ASSERT_MSG_EQ(filter.match(packet), false,
                          "UDP filter should not match TCP packet");
  }

  void TestFilterElement() {
    Protocol protocolFilter(UDP_PROTOCOL);
    Ptr<Packet> packet = Create<Packet>();

    UdpHeader udpHeader;
    packet->AddHeader(udpHeader);

    Ipv4Header ipHeader;
    ipHeader.SetProtocol(UDP_PROTOCOL);
    packet->AddHeader(ipHeader);

    NS_TEST_ASSERT_MSG_EQ(protocolFilter.match(packet), true,
                          "UDP filter element should match UDP packet");
  }

  void TestDestIpMaskFilter() {
    Filter filter;
    Ptr<Packet> packet = Create<Packet>();

    Ipv4Header ipHeader;
    ipHeader.SetDestination(Ipv4Address("10.1.1.2"));
    packet->AddHeader(ipHeader);

    FilterElement *destIpMaskFilter =
        new DestIpMask(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"));
    filter.elements.push_back(destIpMaskFilter);

    NS_TEST_ASSERT_MSG_EQ(filter.match(packet), true,
                          "DestIpMask filter should match the destination IP");
  }

  void TestDestPortFilter() {
    Filter filter;
    Ptr<Packet> packet = Create<Packet>();

    Ipv4Header ipHeader;
    ipHeader.SetProtocol(UDP_PROTOCOL);
    packet->AddHeader(ipHeader);

    UdpHeader udpHeader;
    udpHeader.SetDestinationPort(5000);
    packet->AddHeader(udpHeader);

    FilterElement *destPortFilter = new DestPort(5000);
    filter.elements.push_back(destPortFilter);

    NS_TEST_ASSERT_MSG_EQ(filter.match(packet), true,
                          "DestPort filter should match the destination port");
  }

  void TestSourceIpMaskFilter() {
    Filter filter;
    Ptr<Packet> packet = Create<Packet>();

    Ipv4Header ipHeader;
    ipHeader.SetSource(Ipv4Address("10.1.1.2"));
    packet->AddHeader(ipHeader);

    FilterElement *sourceIpMaskFilter =
        new SourceIpMask(Ipv4Address("10.1.1.0"), Ipv4Mask("255.255.255.0"));
    filter.elements.push_back(sourceIpMaskFilter);

    NS_TEST_ASSERT_MSG_EQ(filter.match(packet), true,
                          "SourceIpMask filter should match the source IP");
  }

  void TestSourcePortFilter() {
    Filter filter;
    Ptr<Packet> packet = Create<Packet>();

    Ipv4Header ipHeader;
    ipHeader.SetProtocol(UDP_PROTOCOL);
    packet->AddHeader(ipHeader);

    UdpHeader udpHeader;
    udpHeader.SetSourcePort(5000);
    packet->AddHeader(udpHeader);

    FilterElement *sourcePortFilter = new SourcePort(5000);
    filter.elements.push_back(sourcePortFilter);

    NS_TEST_ASSERT_MSG_EQ(filter.match(packet), true,
                          "SourcePort filter should match the source port");
  }
};

/**
 * \ingroup diff-serv-tests
 * TestSuite for module diff-serv
 */
class DiffServTestSuite : public TestSuite {
public:
  DiffServTestSuite();
};

DiffServTestSuite::DiffServTestSuite() : TestSuite("diff-serv", UNIT) {
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  AddTestCase(new FilterTestCase, TestCase::QUICK);
  AddTestCase(new SpqTestCase, TestCase::QUICK);
  AddTestCase(new ReadConfigFileTestCase, TestCase::QUICK);
}

/**
 * \ingroup diff-serv-tests
 * Static variable for test initialization
 */
static DiffServTestSuite sdiffServTestSuite;
