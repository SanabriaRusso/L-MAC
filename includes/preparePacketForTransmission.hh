#include <math.h>
#include <algorithm>

#define AC 4

void preparePacketForTransmission(Packet &packet, int id, double txTime)
{
	packet.source = id;
	packet.tx_time = txTime;
}