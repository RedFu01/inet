//
// Copyright (C) 2006 Andras Varga
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
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#ifndef __INET_IEEE80211MGMTAPBASE_H
#define __INET_IEEE80211MGMTAPBASE_H

#include "inet/common/INETDefs.h"

#include "inet/common/packet/Packet.h"
#include "inet/linklayer/ieee80211/mgmt/Ieee80211MgmtBase.h"

namespace inet {

class EtherFrame;

namespace ieee80211 {

/**
 * Used in 802.11 infrastructure mode: abstract base class for management frame
 * handling for access points (APs). This class extends Ieee80211MgmtBase
 * with utility functions that are useful for implementing AP functionality.
 *
 * @author Andras Varga
 */
class INET_API Ieee80211MgmtAPBase : public Ieee80211MgmtBase
{
  public:
    typedef enum { ENCAP_DECAP_TRUE = 1, ENCAP_DECAP_FALSE, ENCAP_DECAP_ETH } EncapDecap;

  protected:
    bool isConnectedToHL;
    EncapDecap encapDecap;

  protected:
    virtual int numInitStages() const override { return NUM_INIT_STAGES; }
    virtual void initialize(int) override;

    /**
     * Utility function for APs: sends back a data frame we received from a
     * STA to the wireless LAN, after tweaking fromDS/toDS bits and shuffling
     * addresses as needed.
     */
    virtual void distributeReceivedDataFrame(Ieee80211DataFrame *frame);

    /** Utility function for handleUpperMessage() */
    virtual Ieee80211DataFrame *encapsulate(Packet *msg);

    /**
     * Utility function: converts EtherFrame to Ieee80211Frame. This is needed
     * because MACRelayUnit which we use for LAN bridging functionality deals
     * with EtherFrames.
     */
    virtual Ieee80211DataFrame *convertFromEtherFrame(Packet *packet);

    /**
     * Utility function: converts Ieee80211Frame to EtherFrame. This is needed
     * because MACRelayUnit which we use for LAN bridging functionality deals
     * with EtherFrames.
     */
    virtual Packet *convertToEtherFrame(Ieee80211DataFrame *frame);

    /**
     * Utility function: send a frame to upperLayerOut.
     * If convertToEtherFrameFlag is true, converts the given frame to EtherFrame, deleting the
     * original frame, and send the converted frame.
     * This function is needed for LAN bridging functionality:
     * MACRelayUnit deals with EtherFrames.
     */
    virtual void sendToUpperLayer(Ieee80211DataFrame *frame);
};

} // namespace ieee80211

} // namespace inet

#endif // ifndef __INET_IEEE80211MGMTAPBASE_H

