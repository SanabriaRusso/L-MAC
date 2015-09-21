#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <string>
#include <array>
#include <map>
#include <bitset>
#include "Aux.h"
#include "FIFO.h"
// #include "includes/dumpStationLog.hh"


//Suggested value is MAXSTAGE+1
#define MAX_RET 7


using namespace std;

component STA : public TypeII
{
    public:
        void Setup();
        void Start();
        void Stop();

    public:
    	//Node-specific characteristics
        

    private:
    	

    public:
        //Connections
        inport void inline in_slot(SLOT_notification &slot);
        inport void inline in_packet(Packet &packet);
        outport void out_packet(Packet &packet);
};

void STA :: Setup()
{

};

void STA :: Start()
{

	
};

void STA :: Stop()
{

    
};

void STA :: in_slot(SLOT_notification &slot)
{	


};

void STA :: in_packet(Packet &packet)
{

}

