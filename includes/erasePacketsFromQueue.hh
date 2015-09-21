#include "../Aux.h"

using namespace std;

void erasePacketsFromQueue(FIFO <Packet> &Q, double &erased)
{
    erased++;
    Q.DelFirstPacket();
}
