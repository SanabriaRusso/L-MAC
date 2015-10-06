#include "../Aux.h"
using namespace std;

void pickNewPacket(Packet &packet, FIFO <Packet> &Q, int id, double pickupTime)
{
	int alwaysSaturated = 1;

	if(alwaysSaturated == 0) packet = Q.GetFirstPacket();
	packet.contention_time = pickupTime;

}
