#include "../Aux.h"
using namespace std;

void pickNewPacket(Packet &packet, FIFO <Packet> &Q, int id, double pickupTime)
{
	packet = Q.GetFirstPacket();
	packet.contention_time = pickupTime;

}
