/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

// Network topology
//
//     40kb/s, 10ms       10kb/s, 10ms
// n0-----------------n1-----------------n2

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/spq.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("SpqValidation");

int main(int argc, char *argv[]) {
  // Parse command line arguments
  CommandLine cmd(__FILE__);
  // this config path is temporal, you should pass the configPath
  // as an argument when running the program. If the relative path
  // doesn't work for you, modify it to absolute path instead.
  std::string configPath = "contrib/diff-serv/examples/config.json";
  cmd.AddValue("configPath", "Path to queue config file", configPath);
  cmd.Parse(argc, argv);

  // Time scale and component logging
  Time::SetResolution(Time::MS);
  LogComponentEnable("Spq", LOG_LEVEL_ALL);
  LogComponentEnable("SpqValidation", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

  // Here, we will explicitly create three nodes.  The first container contains
  // nodes 0 and 1 from the diagram above, and the second one contains nodes
  // 1 and 2.  This reflects the channel connectivity, and will be used to
  // install the network interfaces and connect them with a channel.
  NodeContainer n0n1;
  n0n1.Create(2);
  Ptr<Node> n0 = n0n1.Get(0);
  Ptr<Node> n1 = n0n1.Get(1);

  NodeContainer n1n2;
  n1n2.Add(n0n1.Get(1));
  n1n2.Create(1);
  Ptr<Node> n2 = n1n2.Get(1);

  // We create the channels first without any IP addressing information
  // First make and configure the helper, so that it will put the appropriate
  // attributes on the network interfaces and channels we are about to install.
  PointToPointHelper p2p;
  p2p.SetDeviceAttribute("DataRate", DataRateValue(DataRate(40000)));
  p2p.SetChannelAttribute("Delay", TimeValue(MilliSeconds(10)));

  // And then install devices and channels connecting our topology.
  NetDeviceContainer dev0 = p2p.Install(n0n1);
  p2p.SetDeviceAttribute("DataRate", DataRateValue(DataRate(10000)));
  NetDeviceContainer dev1 = p2p.Install(n1n2);

  // Now add ip/tcp stack to all nodes.
  InternetStackHelper internet;
  internet.InstallAll();

  // Later, we add IP addresses.
  Ipv4AddressHelper ipv4;
  ipv4.SetBase("10.1.0.0", "255.255.255.0");
  ipv4.Assign(dev0);
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer ipInterfs = ipv4.Assign(dev1);

  // Add Spq to device 1-1
  // Get the device going from n1 to n2
  Ptr<NetDevice> baseDev = dev1.Get(0);
  Ptr<PointToPointNetDevice> n1_1dev =
      baseDev->GetObject<PointToPointNetDevice>();

  // Instantiate an SPQ
  Ptr<Spq> spqPtr = CreateObject<Spq>();
  std::vector<TrafficClass *> trafficClasses =
      spqPtr->ReadConfigFile(configPath);
  spqPtr->SetTrafficClasses(trafficClasses);
  n1_1dev->SetQueue(spqPtr);

  // Add in the applications
  uint16_t hiPort = 2003;
  uint16_t lowPort = 2001;
  double stopTime = 10.0;
  double serverListenStartTime = 1.0;
  double lowPriStartTime = 2.0;
  double hiPriStartTime = 5.0;

  NS_LOG_INFO("Create UdpServer (listener) applications on node 2.");
  // Set up the sinks at n2
  UdpServerHelper hiServer(hiPort);
  ApplicationContainer apps = hiServer.Install(n2);
  apps.Start(Seconds(serverListenStartTime));
  apps.Stop(Seconds(stopTime));

  UdpServerHelper lowServer(lowPort);
  apps = lowServer.Install(n2);
  apps.Start(Seconds(serverListenStartTime));
  apps.Stop(Seconds(stopTime));

  NS_LOG_INFO(
      "Create UdpClient (sender) applications on node 0 to send to node 2.");
  uint32_t MaxPacketSize = 1000;
  Time interPacketInterval = Seconds(0.3);
  uint32_t maxPacketCount = 200;
  // Dest is device on n2
  UdpClientHelper client(ipInterfs.GetAddress(1), hiPort);
  client.SetAttribute("MaxPackets", UintegerValue(maxPacketCount));
  client.SetAttribute("Interval", TimeValue(interPacketInterval));
  client.SetAttribute("PacketSize", UintegerValue(MaxPacketSize));
  // Install on n0
  apps = client.Install(n0);
  apps.Start(Seconds(hiPriStartTime));
  apps.Stop(Seconds(stopTime));
  // Low priority port
  client.SetAttribute("RemotePort", UintegerValue(lowPort));
  apps = client.Install(n0);
  apps.Start(Seconds(lowPriStartTime));
  apps.Stop(Seconds(stopTime));

  // and setup ip routing tables to get total ip-level connectivity.
  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  // Capture packet transfers
  p2p.EnablePcapAll("p3");

  // Run the simulation
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}
