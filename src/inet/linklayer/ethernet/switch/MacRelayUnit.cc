// Copyright (C) 2013 OpenSim Ltd.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//


#include "inet/common/IProtocolRegistrationListener.h"
#include "inet/common/LayeredProtocolBase.h"
#include "inet/common/ModuleAccess.h"
#include "inet/common/Protocol.h"
#include "inet/common/ProtocolTag_m.h"
#include "inet/common/lifecycle/NodeOperations.h"
#include "inet/linklayer/common/InterfaceTag_m.h"
#include "inet/linklayer/ethernet/EtherFrame_m.h"
#include "inet/linklayer/ethernet/EtherMacBase.h"
#include "inet/linklayer/ethernet/Ethernet.h"
#include "inet/linklayer/ethernet/switch/MacRelayUnit.h"

namespace inet {

Define_Module(MacRelayUnit);

void MacRelayUnit::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        numProcessedFrames = numDiscardedFrames = 0;

        macTable = getModuleFromPar<IMacAddressTable>(par("macTableModule"), this);
        ifTable = getModuleFromPar<IInterfaceTable>(par("interfaceTableModule"), this);

        WATCH(numProcessedFrames);
        WATCH(numDiscardedFrames);
    }
    else if (stage == INITSTAGE_NETWORK_INTERFACE_CONFIGURATION) {
        NodeStatus *nodeStatus = dynamic_cast<NodeStatus *>(findContainingNode(this)->getSubmodule("status"));
        isOperational = (!nodeStatus) || nodeStatus->getState() == NodeStatus::UP;
        registerService(Protocol::ethernetMac, nullptr, gate("ifIn"));
        registerProtocol(Protocol::ethernetMac, gate("ifOut"), nullptr);
    }
}

void MacRelayUnit::handleMessage(cMessage *msg)
{
    if (!isOperational) {
        EV_ERROR << "Message '" << msg << "' arrived when module status is down, dropped it." << endl;
        delete msg;
        return;
    }
    else if (msg->isSelfMessage())
        throw cRuntimeError("This module doesn't handle self-messages!");

    // messages from network
    else if (msg->arrivedOn("ifIn")) {
        Packet *packet = check_and_cast<Packet *>(msg);
        // Frame received from MAC unit
        emit(packetReceivedFromLowerSignal, packet);
        handleAndDispatchFrame(packet);
    }
    // unknown gate
    else
        throw cRuntimeError("Message arrived on unknown gate");
}

void MacRelayUnit::broadcast(Packet *packet, int arrivalInterfaceId)
{
    EV_DETAIL << "Broadcast frame " << packet << endl;

    auto oldPacketProtocolTag = packet->removeTag<PacketProtocolTag>();
    packet->clearTags();
    auto newPacketProtocolTag = packet->addTag<PacketProtocolTag>();
    *newPacketProtocolTag = *oldPacketProtocolTag;
    delete oldPacketProtocolTag;
    packet->trim();

    int numPorts = ifTable->getNumInterfaces();
    for (int i = 0; i < numPorts; i++) {
        InterfaceEntry *ie = ifTable->getInterface(i);
        if (ie->isLoopback() || !ie->isBroadcast())
            continue;
        int ifId = ie->getInterfaceId();
        if (arrivalInterfaceId != ifId) {
            Packet *dupFrame = packet->dup();
            dupFrame->addTagIfAbsent<InterfaceReq>()->setInterfaceId(ifId);
            emit(packetSentToLowerSignal, dupFrame);
            send(dupFrame, "ifOut");
        }
    }
    delete packet;
}

void MacRelayUnit::handleAndDispatchFrame(Packet *packet)
{
    //FIXME : should handle multicast mac addresses correctly

    const auto& frame = packet->peekAtFront<EthernetMacHeader>();
    int arrivalInterfaceId = packet->getTag<InterfaceInd>()->getInterfaceId();

    numProcessedFrames++;

    // update address table
    learn(frame->getSrc(), arrivalInterfaceId);

    // handle broadcast frames first
    if (frame->getDest().isBroadcast()) {
        EV << "Broadcasting broadcast frame " << frame << endl;
        broadcast(packet, arrivalInterfaceId);
        return;
    }

    // Finds output port of destination address and sends to output port
    // if not found then broadcasts to all other ports instead
    int outputInterfaceId = macTable->getPortForAddress(frame->getDest());
    // should not send out the same frame on the same ethernet port
    // (although wireless ports are ok to receive the same message)
    if (arrivalInterfaceId == outputInterfaceId) {
        EV << "Output port is same as input port, " << packet->getFullName()
           << " dest " << frame->getDest() << ", discarding frame\n";
        numDiscardedFrames++;
        delete packet;
        return;
    }

    if (outputInterfaceId >= 0) {
        EV << "Sending frame " << frame << " with dest address " << frame->getDest() << " to port " << outputInterfaceId << endl;
        auto oldPacketProtocolTag = packet->removeTag<PacketProtocolTag>();
        packet->clearTags();
        auto newPacketProtocolTag = packet->addTag<PacketProtocolTag>();
        *newPacketProtocolTag = *oldPacketProtocolTag;
        delete oldPacketProtocolTag;
        packet->addTagIfAbsent<InterfaceReq>()->setInterfaceId(outputInterfaceId);
        packet->trim();
        emit(packetSentToLowerSignal, packet);
        send(packet, "ifOut");
    }
    else {
        EV << "Dest address " << frame->getDest() << " unknown, broadcasting frame " << packet << endl;
        broadcast(packet, arrivalInterfaceId);
    }
}

void MacRelayUnit::start()
{
    macTable->clearTable();
    isOperational = true;
}

void MacRelayUnit::stop()
{
    macTable->clearTable();
    isOperational = false;
}

bool MacRelayUnit::handleOperationStage(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    Enter_Method_Silent();

    if (dynamic_cast<NodeStartOperation *>(operation)) {
        if (static_cast<NodeStartOperation::Stage>(stage) == NodeStartOperation::STAGE_LINK_LAYER) {
            start();
        }
    }
    else if (dynamic_cast<NodeShutdownOperation *>(operation)) {
        if (static_cast<NodeShutdownOperation::Stage>(stage) == NodeShutdownOperation::STAGE_LINK_LAYER) {
            stop();
        }
    }
    else if (dynamic_cast<NodeCrashOperation *>(operation)) {
        if (static_cast<NodeCrashOperation::Stage>(stage) == NodeCrashOperation::STAGE_CRASH) {
            stop();
        }
    }
    else {
        throw cRuntimeError("Unsupported operation '%s'", operation->getClassName());
    }

    return true;
}

void MacRelayUnit::learn(MacAddress srcAddr, int arrivalInterfaceId)
{
    macTable->updateTableWithAddress(arrivalInterfaceId, srcAddr);
}

void MacRelayUnit::finish()
{
    recordScalar("processed frames", numProcessedFrames);
    recordScalar("discarded frames", numDiscardedFrames);
}

} // namespace inet

