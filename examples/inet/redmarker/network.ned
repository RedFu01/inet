//
// author: Marcel Marek
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
// along with this program.  If not, see <https://www.gnu.org/licenses/>.// 

package inet.examples.inet.redmarker;


import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
//import inet.node.ethernet.EthernetSwitch;
import inet.node.inet.Router;
//import inet.node.dmpr.DmprRouter;
//import inet.nodes.inet.Router;
//import inet.node.ospfv2.OspfRouter;
import inet.node.inet.StandardHost;
//import inet.nodes.inet.StandardHost;
import ned.DatarateChannel;

//import inet.linklayer.queue.REDDropper;

//import inet.queueing.IPacketQueue;
//import inet.linklayer.queue.DropTailQueue;
//import inet.queueing.FifoQueue;
//import inet.queueing.RedDropper;
//import inet.queueing.RedDropperQueue;



//import inet.visualizer.canvas.integrated.IntegratedCanvasVisualizer;


channel NormalP extends DatarateChannel
{
    parameters:
        datarate = default(10Mbps);
        delay = default(1ms);
        per = 0;
        ber = 0;
}

channel FastP extends DatarateChannel
{
    parameters:
        datarate = 100Mbps;
        delay = 1ms;
        per = 0;
        ber = 0;
}




network RedMarkerNetwork
{
    parameters:
        @display("bgb=4166.738,3051.564,white;bgl=8");

    submodules:


        routerCore1: Router {
            parameters:
                @display("p=1026.4801,995.93005;i=abstract/router");

        }
        routerCore2: Router {
            parameters:
                @display("p=2945.02,995.93005;i=abstract/router");
        }

        configurator: Ipv4NetworkConfigurator;


        sender1: StandardHost {
            parameters:
                @display("p=342.16,995.93005");
        }

        receiver1: StandardHost {
            parameters:
                @display("p=3708.7703,995.93005");
        }

    connections:

        sender1.ethg++ <--> FastP <--> routerCore1.ethg++;
        receiver1.ethg++ <--> FastP <--> routerCore2.ethg++;
        routerCore1.ethg++ <--> NormalP <--> routerCore2.ethg++;


}




