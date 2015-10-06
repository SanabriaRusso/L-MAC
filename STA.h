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
#include "includes/pickNewPacket.hh"
#include "includes/computeBackoff.hh"
#include "includes/erasePacketsFromQueue.hh"
#include "includes/preparePacketForTransmission.hh"
#include "includes/dumpStationLog.hh"


//Suggested value is MAXSTAGE+1
#define MAXSTAGE 5
#define MAX_RET 6


using namespace std;

component STA : public TypeII
{
    public:
        void Setup();
        void Start();
        void Stop();

    public:

        int nodesInSim; //Number of nodes in the whole simulation

    	//Node-specific characteristics
        int id;
        int K;  //max queue size
        int protocol;   //0 = DCF; 1 = L-MAC;
        int L;  //Packet length

        //Queue management
        double inPackets;
        double admittedPackets;
        double erased; //Erased from the queue
        double dropped;
        int isAlwaysSaturated;

        //Statistics
        double accummTimeBetSxTx;
        double throughput;
        double collisions;
        ofstream statistics;

    private:
    	//Contention
        double backoffCounter;
        int backoffStage;
        int ITx;    //I transmitted: 0 = no; 1 = yes

        //Transmissions
        double tx;
        double sxTx;    //Successfull transmissions
        double colTx;   //Collisions
        int ack;     //Result of the transmission
        int backlog;    //Is there something in the queue?
        int retAttempt;

        //Private queue management
        Packet packet;
        FIFO <Packet> MAC;  //The MAC queue


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
    inPackets = 0;
    admittedPackets = 0;
    erased = 0;
    dropped = 0;
    backoffCounter = 0;
    backoffStage = 0;
    tx = 0;
    sxTx = 0;
    colTx = 0;
    backlog = 0;
    ITx = 0;
    retAttempt = 0;
    ack = 0;
    isAlwaysSaturated = 1;

    //Packet details
    packet.L = L;
    packet.source = id;
	

    //Statistics
    accummTimeBetSxTx = 0;
};

void STA :: Stop()
{

    //Gathering data from last packet before stop
    if(backlog == 1) accummTimeBetSxTx += double(SimTime() - packet.contention_time);

    cout << "-Station-" << id << ":" << endl;
    if(admittedPackets - erased - MAC.QueueSize() == 0 || isAlwaysSaturated == 1){
        cout << "\tTotal throughput: " << sxTx*L*8.0 / SimTime() << endl;
        cout << "\tCollisions: " << colTx/tx << endl;
        cout << "\tReceived: " << inPackets << " packets" << endl;
        cout << "\tDropped: " << dropped << " packets" << endl;
        cout << "\tAvg. seconds between sxTx " << accummTimeBetSxTx / sxTx << endl;

        //Statistics
        throughput = sxTx*L*8.0 / SimTime();
        collisions = colTx/tx;
        accummTimeBetSxTx /= sxTx;

        //Creating the output file
        string logName = "sta-" + to_string(id) + ".log";        
        string routeToFile = "Results/stations/" + logName;
        statistics.open(routeToFile.c_str(), ios::app);
        dumpStationLog(statistics,nodesInSim,throughput,collisions,accummTimeBetSxTx/sxTx);
        statistics.close();

    }else{
        cout << "Something's wrong with either the erasure or admittance of packets" << endl;
    }

};

void STA :: in_slot(SLOT_notification &slot)
{	
    switch(slot.status)
    {
        //Empty slot
        case 0:
            if(backlog == 1){
                if(backoffCounter > 0){
                    backoffCounter--;
                }
            }else{
                if(MAC.QueueSize() > 0)
                {
                    backlog = 1;
                    pickNewPacket(packet,MAC,id,SimTime());
                    computeBackoff(backoffStage,MAXSTAGE,backoffCounter,backlog,ack);
                }
            }
            break;
        //Successful transmission
        case 1:
            //If this station transmitted in observed slot,
            //we assign a new backoff counter
            if(ITx == 1){
                sxTx++;
                ack = 1;
                accummTimeBetSxTx += double(SimTime() - packet.contention_time);
                erasePacketsFromQueue(MAC,erased,isAlwaysSaturated);
                ITx = 0;    //resetting to zero
                backoffStage = 0;   //resetting backoff stage for DCF
                backlog = 0;
                if(MAC.QueueSize() > 0) backlog = 1;
                if(backlog == 1) pickNewPacket(packet,MAC,id,SimTime());
                computeBackoff(backoffStage,MAXSTAGE,backoffCounter,backlog,ack);
                ack = 0;
            }else
            //If we observe a sxTx slot, we decrement the counter.
            //Only the sta involved in the sxTx will not decrement it.
            //This ensures fairness by preventing the sx sta from tx again
            //drawing a backoff equal to zero.
            {
                backoffCounter--;
            }
            break;
        default:
            if(ITx == 1){
                colTx++;
                retAttempt++;
                backoffStage = min(backoffStage + 1,MAXSTAGE);
                ITx = 0;
                if(retAttempt == MAX_RET){
                    accummTimeBetSxTx = double(SimTime() - packet.contention_time);
                    erasePacketsFromQueue(MAC,erased,isAlwaysSaturated);
                    dropped++;
                    backoffStage = 0;   //Resetting due to drop
                    retAttempt = 0;
                    backlog = 0;
                    if(MAC.QueueSize() > 0) backlog = 1;
                    if(backlog == 1) pickNewPacket(packet,MAC,id,SimTime());
                }
                computeBackoff(backoffStage,MAXSTAGE,backoffCounter,backlog,ack);
            }else{
                if(backoffCounter > 0){
                    backoffCounter--;
                }
            }
            break;
    }

    //Transmission
    if(backoffCounter == 0){
        preparePacketForTransmission(packet,id,SimTime());
        tx++;
        ITx = 1;
        // cout << SimTime() << " transmitting" << endl;
        out_packet(packet);
    }
};

void STA :: in_packet(Packet &packet)
{
    inPackets++;
    if(MAC.QueueSize() < K)   //is there space in the node's queue?
    {
        admittedPackets++;
        packet.queuing_time = SimTime();
        MAC.PutPacket(packet);
    }

}

