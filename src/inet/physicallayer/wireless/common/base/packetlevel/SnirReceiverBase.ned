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

package inet.physicallayer.wireless.common.base.packetlevel;

import inet.physicallayer.wireless.common.contract.packetlevel.IReceiver;

//
// This module servces as a base module for SNIR receiver models.
//
module SnirReceiverBase like IReceiver
{
    parameters:
        double snirThreshold @unit(dB);    // reception is not successful if the SNIR is below this threshold (unsuccessful reception)
        string snirThresholdMode @unit("min","mean") = default("min");
        @display("i=block/wrx");
}

