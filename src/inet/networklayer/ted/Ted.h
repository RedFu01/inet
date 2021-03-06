//
// Copyright (C) 2005 Vojtech Janota
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#ifndef __INET_TED_H
#define __INET_TED_H

#include "inet/networklayer/rsvpte/IntServ_m.h"
#include "inet/networklayer/ted/Ted_m.h"
#include "inet/routing/base/RoutingProtocolBase.h"

namespace inet {

class IIpv4RoutingTable;
class IInterfaceTable;
class NetworkInterface;

/**
 * Contains the Traffic Engineering Database and provides public methods
 * to access it from MPLS signalling protocols (LDP, RSVP-TE).
 *
 * See NED file for more info.
 */
class INET_API Ted : public RoutingProtocolBase
{
  public:
    /**
     * Only used internally, during shortest path calculation:
     * vertex in the graph we build from links in TeLinkStateInfoVector.
     */
    struct vertex_t {
        Ipv4Address node; // FIXME *** is this the routerID? ***
        int parent; // index into the same vertex_t vector
        double dist; // distance to root (???)
    };

    /**
     * Only used internally, during shortest path calculation:
     * edge in the graph we build from links in TeLinkStateInfoVector.
     */
    struct edge_t {
        int src; // index into the vertex_t[] vector
        int dest; // index into the vertex_t[] vector
        double metric; // link cost
    };

    /**
     * The link state database. (TeLinkStateInfoVector is defined in Ted.msg)
     */
    TeLinkStateInfoVector ted;

  public:
    Ted();
    virtual ~Ted();

  protected:
    virtual void initialize(int stage) override;
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void handleMessageWhenUp(cMessage *msg) override;

    virtual void initializeTED();

    virtual Ipv4AddressVector calculateShortestPath(Ipv4AddressVector dest,
            const TeLinkStateInfoVector& topology, double req_bandwidth, int priority);

  public:
    /** @name Public interface to the Traffic Engineering Database */
    //@{
    virtual Ipv4Address getInterfaceAddrByPeerAddress(Ipv4Address peerIP);
    virtual Ipv4Address peerRemoteInterface(Ipv4Address peerIP);
    virtual Ipv4Address getPeerByLocalAddress(Ipv4Address localInf);
    virtual Ipv4Address primaryAddress(Ipv4Address localInf);
    virtual bool isLocalPeer(Ipv4Address inetAddr);
    virtual bool isLocalAddress(Ipv4Address addr);
    virtual unsigned int linkIndex(Ipv4Address localInf);
    virtual unsigned int linkIndex(Ipv4Address advrouter, Ipv4Address linkid);
    virtual Ipv4AddressVector getLocalAddress();

    virtual void rebuildRoutingTable();
    //@}

    virtual void handleStartOperation(LifecycleOperation *operation) override;
    virtual void handleStopOperation(LifecycleOperation *operation) override;
    virtual void handleCrashOperation(LifecycleOperation *operation) override;

  protected:
    IIpv4RoutingTable *rt = nullptr;
    IInterfaceTable *ift = nullptr;
    Ipv4Address routerId;

    Ipv4AddressVector interfaceAddrs; // list of local interface addresses

    int maxMessageId = 0;

  protected:
    virtual int assignIndex(std::vector<vertex_t>& vertices, Ipv4Address nodeAddr);

    std::vector<vertex_t> calculateShortestPaths(const TeLinkStateInfoVector& topology,
            double req_bandwidth, int priority);

  public: // FIXME
    virtual bool checkLinkValidity(TeLinkStateInfo link, TeLinkStateInfo *& match);
    virtual void updateTimestamp(TeLinkStateInfo *link);
};

std::ostream& operator<<(std::ostream& os, const TeLinkStateInfo& info);

} // namespace inet

#endif

