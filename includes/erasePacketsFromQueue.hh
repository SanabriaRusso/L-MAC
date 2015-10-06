#include "../Aux.h"

using namespace std;

void erasePacketsFromQueue(FIFO <Packet> &Q, double &erased, int alwaysSaturated)
{
    erased++;
    if(alwaysSaturated == 0) Q.DelFirstPacket();
}
