//
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
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//

#ifndef __INET_SCALARTRANSMISSION_H
#define __INET_SCALARTRANSMISSION_H

#include "inet/physicallayer/wireless/common/base/packetlevel/FlatTransmissionBase.h"

namespace inet {

namespace physicallayer {

class INET_API ScalarTransmission : public FlatTransmissionBase, public virtual IScalarSignal
{
  protected:
    const W power;

  public:
    ScalarTransmission(const IRadio *transmitter, const Packet *packet, const simtime_t startTime, const simtime_t endTime, const simtime_t preambleDuration, const simtime_t headerDuration, const simtime_t dataDuration, const Coord& startPosition, const Coord& endPosition, const Quaternion& startOrientation, const Quaternion& endOrientation, const IModulation *modulation, b headerLength, b dataLength, Hz centerFrequency, Hz bandwidth, bps bitrate, W power);

    virtual std::ostream& printToStream(std::ostream& stream, int level, int evFlags = 0) const override;

    virtual W getPower() const override { return power; }
    virtual W computeMinPower(const simtime_t startTime, const simtime_t endTime) const override { return power; }
};

} // namespace physicallayer

} // namespace inet

#endif

